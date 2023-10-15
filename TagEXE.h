// TagEXE.h : Include file for standard system include files,
// or project specific include files.

#pragma once
#include <iostream>

// TODO: Reference additional headers your program requires here.
#include <ctype.h>

#define MAJOR_BUILD		"1"
#define MINOR_BUILD		"0"
#define PATCH_BUILD		"0"
#define RELEASE_BUILD	"157"
#define DOT				"."
#define VERSION_BUILD MAJOR_BUILD DOT MINOR_BUILD DOT PATCH_BUILD DOT RELEASE_BUILD
//---------------------------------------------------------------------------
char g_szTemplateFile[256] = "\0";
char g_szTemplateText[256] = "\0";

//Our message tag to include in destination file
char sMsgTagData[256] = { '\0' };

//Temporary storage units
char* szInput;
char* sFileName;

//What type of file is it?
int FileType;
char sFileType[25] = { '\0' };

//If user wants to use the -r (read message tags only) option...
bool bReadTag = false;
bool bReadLinerTag = false;

bool bBinary = true;

//Function to examine file types.
int ExamineFileType(char* sFileName);
void HelpFunction(void);
int GetCommandLineArgs(int argc, char* argv[]);
void DisplayMsgTag();


//June 23, 2003
int ExamineFileType(char* szFileName)
{
    FILE* hMainFile;
    int iType = 0;
    char szHeader[8] = { '\0' };
    //char cTestChar;
    errno_t err;

    //TODO: Consider passing the file pointer once opened. No need to open/close file

    if ((err = fopen_s(&hMainFile, szFileName, "r+b")) != 0)
    {
        fprintf(stderr, "Cannot open %s. File is invalid, write-protected, or opened.\n", szFileName);
        return 0;
    }

    fread(&szHeader, strlen(szHeader) + 1, 8, hMainFile);

    if (szHeader[0] == 'M' && szHeader[1] == 'Z')
    {
        if (szHeader[2] == 'P')
        {
            fclose(hMainFile);
            return (1000); //EXE
        }
    }

    if (szHeader[0] == 'M' && szHeader[1] == 'Z')
    {
        fclose(hMainFile);
        return (1000); //EXE
    }


    if (szHeader[0] == 'B' && szHeader[1] == 'M')
    {
        //if (szHeader[2] == '�')
        //{
        fclose(hMainFile);
        return (1002);
        //}
    }

    //CHEAP way for MS Word Doc.
    //if (szHeader[0] == '�' && szHeader[1] == '�')
    //{
    //    if (szHeader[2] == '')
    //    {
    //        fclose(hMainFile);
    //        return (1003);
    //    }
    //}

    //if (szHeader[0] == '�' && szHeader[1] == '�')
    //{
    //    if (szHeader[2] == '')
    //    {
    //        fclose(hMainFile);
    //        return (1003);
    //    }
    //}


    //GIF files.
    if (szHeader[0] == 'G' && szHeader[1] == 'I' && szHeader[2] == 'F' &&
        szHeader[3] == '8' && szHeader[4] == '9' && szHeader[5] == 'a' &&
        szHeader[6] == 'd')
    {
        fclose(hMainFile);
        return (1004);
    }

    //Jpg files.
    if (szHeader[6] == 'J' && szHeader[7] == 'F')
    {
        fclose(hMainFile);
        return (1005);
    }

    if (szHeader[0] == 'P' && szHeader[1] == 'K')
    {
        fclose(hMainFile);
        return (1006);
    }

    //THIS HAS to be last!
    for (int iCount = 0; iCount < strlen(szHeader); iCount++)
    {
        if (isprint(szHeader[iCount]))
        {
            fclose(hMainFile);
            return (1001); //TXT
        }
    }

    fclose(hMainFile);
    return 0;
}

//June 23, 2003
void HelpFunction(void)
{
    printf("\n\n");
    printf("TagEXE  V%s\n", VERSION_BUILD);
    printf("by C. Winters (thechriswinters@gmail.com)\n");
    printf("Copyright, (c) 2003\n");
    printf("\nTags EXE's, DLLs, or anyfile using string or template files.\n");
    printf("TagEXE will insert a string up to 256 characters at the end of\n");
    printf("your finished build product.\n");
    printf("This can be used to customize, stamp, registration purposes or\n");
    printf("tamper proof binaries.\n");
    printf("\nUsage: tagexe [-t \"{string}\"] [-f {template.fil}] {sourcefile}\n\n");
    printf("-f Name of the template file to use. This contains the strings to tag.\n");
    printf("-t The string text to tag instead of template tag file. Use quotes to\n");
    printf("   denote string. *overrides -f*\n");
    printf("-b File is binary\n");
    printf("   not passing this parameter defaults to ASCII files.\n");
    printf("-a File is ASCII- or not binary.\n");
    printf("-r Test and read back tags, if any.\n");
    printf("-o Read back tags, if any, only on one line.\n");
    printf("   \nExamples:\n");
    printf("   tagexe -t \"SomeCompany, (c) Copyright 2003\" filename.exe\n");
    printf("   Inserts the string given at the end of file name filename.exe.\n\n");
    printf("   tagexe -f tagfile.txt filename.exe\n");
    printf("   TagEXE will use a pre-defined data file to insert the tag.\n\n");
    printf("   tagexe -r filename.exe\n");
    printf("   Read back the contents of a tagged file, if present.\n\n");
    printf("\n tagEXE can automatically detect files and change the proper mode for writing.\n");
    printf(" tagEXE supports DOC, JPG, GIF, DLL, EXE, BMP, ZIP, & TXT.\n");
    printf("\n");
}

int GetCommandLineArgs(int argc, char* argv[])
{
    int i;
    for (i = 1; i < argc; i++)
    {
        if ((*argv[i] != '-'))
            szInput = argv[i];

        //someone just put a '-'
        else if (strcmp(argv[i], "-") == 0)
            return (1);

        else if ((strcmp(argv[i], "-?") == 0) || (strcmp(argv[i], "/?") == 0))
        {
            HelpFunction();
            return (1);
        }

        else if (strcmp(argv[i], "-r") == 0)
            bReadTag = true;

        else if (strcmp(argv[i], "-o") == 0)
            bReadLinerTag = true;

        else if (strcmp(argv[i], "-b") == 0)
            bBinary = true;

        else if (strcmp(argv[i], "-a") == 0)
            bBinary = false;

        else if (strcmp(argv[i], "-f") == 0)
        {
            szInput = argv[++i];

            if (szInput == NULL)
            {
                printf("\a\nMissing or incorrect template file. Re-enter.\n");
                return (1);
            }
            else
                strcpy_s(g_szTemplateFile, szInput);
        }

        else if (strcmp(argv[i], "-t") == 0)
        {
            szInput = argv[++i];

            if (szInput == NULL)
            {
                printf("\a\nMissing or incomplete text data. Re-enter.\n");
                return (1);
            }
            else
                strcpy_s(g_szTemplateText, szInput);

            if (strlen(g_szTemplateText) > 256)
            {
                printf("\a\nTagEXE can only support up to 256 chars. Re-enter.\n");
                HelpFunction();
                return (1);
            }
        }
        else
        {
            printf("\a\nNo specifications...\n");
            HelpFunction();
            return (1);
        }
    }

    return (0);
}

bool DetermineFileType(char* sFile)
{
    int iWhatFiletype = ExamineFileType(sFile);

    switch (iWhatFiletype)
    {
    case 0: printf("Cannot open %s. File is invalid, write-protected, or opened.\n", sFile); return (false);
    case 1000: strcpy_s(sFileType, "Executable (.EXE)"); bBinary = true; break;
    case 1001: strcpy_s(sFileType, "Text (.TXT)"); break;
    case 1002: strcpy_s(sFileType, "Bitmap (.BMP)"); bBinary = true; break;
    case 1003: strcpy_s(sFileType, "MS Word Doc (.DOC)"); bBinary = true; break;
    case 1004: strcpy_s(sFileType, "Photo (.GIF)"); bBinary = true; break;
    case 1005: strcpy_s(sFileType, "Photo (.JPG)"); bBinary = true; break;
    case 1006: strcpy_s(sFileType, "Archive (.ZIP)"); bBinary = true; break;
    default:
        strcpy_s(sFileType, "Unknown"); break;
    }

    return(true);
}


FILE * OpenFileAppend(char* sFileName, bool bIsBinary = true)
{
    FILE* fp;
    errno_t err;
    char sMode[5] = {'\0'};

    //ternary operator -> (value ? true : false;)
    bIsBinary ? strcpy_s(sMode, "a+b") : strcpy_s(sMode, "a+t");

    if ((err = fopen_s(&fp, sFileName, sMode)) != 0)
    {
        fprintf(stderr, "Cannot open %s. File is invalid, write-protected, or opened.\n", sFileName);
        fclose(fp);
        return (NULL);
    }
    return (fp);
}

/// <summary>
/// Reads a message tag in a file. Detects magic delimiters and gathers message previously embedded in file.
/// </summary>
/// <param name="fp"></param>
/// <returns>Message data if detected as a string</returns>
char* ReadMsgTag(FILE* fp)
//TODO: In future versions, pass in our guestimate of delimiter amounts.
{
    //Go to end, we're going to get the filelength...
    fseek(fp, 0L, SEEK_END);

    //The length of the source file
    long filelength = ftell(fp);

    //get source file length MINUS the message tag length (pre-set length), plus our delimiter length
    filelength -= (sizeof(g_szTemplateText) + 5);

    //Reset file position back to beginning
    fseek(fp, 0L, SEEK_SET);

    //Set file poistion at the length value to read our message/data.
    fseek(fp, filelength, SEEK_CUR);

    // c -  read a character in buffer
    // iCount -  count how many times we have a set delimiter.
    // bCustomData - if we found our delimiters and custom data, then flag that we found it
    int c, iCCount = 0;
    bool bCustomData = false;

    for (int ch = 0; ch <= 5; ch++)
    {
        c = fgetc(fp);

        // Our delimiter: '\xFE' = 254 UInt8
        if (c == 254) iCCount++;
        
        if (iCCount == 5)
        {
            bCustomData = true; 
            break;
        }
    }

    //TODO fread needs to read the elements correctly. Set at 1 for now should be size of the array
    fread(&g_szTemplateText, sizeof(g_szTemplateText), 1, fp);
    rewind(fp);
    fclose(fp);

    if (strlen(g_szTemplateText) != 0)
        return (g_szTemplateText);
    else
        return (NULL);
}

/// <summary>
/// instead of passing in a string, open a template file to read in a file
/// </summary>
/// <param name="sFileName"></param>
/// <param name="bIsBinary"></param>
/// <returns>Pointer to open template file, NULL if problems</returns>
FILE* OpenTemplateFile(char* sFileName, bool bIsBinary = true)
{
    FILE* hTemplateFile;
    errno_t err;
    char sMode[5] = { '\0' };

    bIsBinary ? strcpy_s(sMode, "r+b") : strcpy_s(sMode, "r+t");

    //No template file? Bail.
    if (strlen(sFileName) == 0)
    {
        printf("No Message tag passed in or template file to read data from. Use -? for help.\n");
        return (NULL);
    }

    if ((err = fopen_s(&hTemplateFile, sFileName, sMode)) != 0)
    {
        printf("Cannot open the template file to gather data!\n");

        if ((hTemplateFile != NULL) || (hTemplateFile != 0))
            fclose(hTemplateFile);

        return (NULL);
    }
    return (hTemplateFile);
}

/// <summary>
/// Instead of passing a string for message tag, read messages in open template file.
/// </summary>
/// <param name="fp"></param>
/// <returns>String of message tag to embed. NULL if nothing was detected</returns>
char* ReadMsgTagTemplateFile(FILE* fp)
{
    //TODO size of template file array is 256. Expand on this for options.
    char szTemplateBufferData[256] = { '\0' };

    fseek(fp, 0L, SEEK_END);
    long filelength = ftell(fp);

    if (filelength > 256)
    {
        printf("\a\nTagEXE can only support up to 256 characters in a message tag. Re-enter or use -? for help.\n");
        return (NULL);
    }

    rewind(fp);
    fread(&szTemplateBufferData, sizeof(szTemplateBufferData), 1, fp);
    fclose(fp);
    //strcpy_s(sMsgTagData, szTemplateBufferData);
    
    if (strlen(szTemplateBufferData) != 0)
        return (szTemplateBufferData);
    else
    {
        printf("\a\nCould not read data from template file. Empty?\n");
        return (NULL);
    }
}

/// <summary>
/// Write message tag contents to open file source pointer
/// </summary>
/// <param name="fp"></param>
/// <returns></returns>
bool WriteMessageTagContents(FILE* fp)
//TODO sMsgTagData is a global variable with a fixed length. LAter versions could detect length.
{
    //Open the file and go to the end of the file. Write contents.
    fseek(fp, sizeof(sMsgTagData), SEEK_END);

    //Put in our five delimiters to later distinguish our message when reading back
    //TODO: Read these in, just added these Jan 3rd 2023
    for (int ch = 0; ch <= 5; ch++)
    {
        fputc('\xFE', fp);
    }

    //write the contents to the file
    if (fwrite(&sMsgTagData, sizeof(sMsgTagData), 1, fp) < 1)
    {
        printf("\a Writing error! LAck of disc space or file corruption?");
        fclose(fp);
        return (false);
    }

    printf("Success! Verifying...\n");
    
    rewind(fp);
    ReadMsgTag(fp);
    DisplayMsgTag();

    //Done!
    if (fp)
        fclose(fp);
}

/// <summary>
/// Displays the message tag in the file if detected.
/// </summary>
void DisplayMsgTag()
{
    int iCount = 0;
    //If we scan the file and look for custom data, there may not be any, but our 
    // read string may contain binary data like a digital siggy.
    bool bBinaryDetected = false;

    for (iCount = 0; iCount < sizeof(g_szTemplateText); iCount++)
    {

        if (!isascii(g_szTemplateText[iCount]))
        {
            bBinaryDetected = true;
            //Immediate break the scan, no need to process all.
            break;
        }
        // else -  ASCII! so no weird binary detected!

        if (g_szTemplateText[iCount] == '\0')
            break;

        //if ((g_szTemplateText[iCount] >= 'a' && g_szTemplateText[iCount] <= 'z') || (g_szTemplateText[iCount] >= 'A' && g_szTemplateText[iCount] <= 'Z'))
        //if ( (g_szTemplateText[iCount] >= 65 && g_szTemplateText[iCount] <= 90) || (g_szTemplateText[iCount] >= 97 && g_szTemplateText[iCount] <= 122) )

    }
    if (!bReadLinerTag)
    {
        printf("TagEXE now reading...\n");
        printf("\n[Source file]   : %s\n", sFileName);
        printf("[File type]     : %s\n", sFileType);

        if (_stricmp(g_szTemplateText, "\0") && bBinaryDetected == false)
            printf("[Custom Message]: %s\n", g_szTemplateText);
        else
            printf("NO CUSTOM MESSAGE TAGGING DETECTED IN THIS FILE!\n");
        //printf("-------------------------------------------------------------------\n");
    }
    else
        printf("%s", g_szTemplateText);
}



