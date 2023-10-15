//---------------------------------------------------------------------------
// TagEXE
// by C. Winters / USA / 
// Original code: June 2003, modded for updates
// 
// Embeds Message tags/data/info at the end of any file.
// Features:
//    - Detects type of file, EXE, TXT, JPG, etc. and writes data (binary/text)
//    - Able to read tags back, if any.
//    - Can use a template data file, or on the fly string tagging.
//
// TODOs
//   - Remove Message Tags
//   - Detect multiple message tags
//   - See TODOs referring to length of message tag. Make them variable.
//
// Help:
//      Tags EXE's, DLLs, or anyfile using string or template files.
//      TagEXE will insert a string up to 256 characters at the end of
//      your finished build product.
//      This can be used to customize, stamp, registration purposes or
//      tamper proof binaries.
//      
//      Usage: tagexe[-t \"{string}\"] [-f {template.fil}] {sourcefile}
//      - f Name of the template file to use.This contains the strings to tag.
//      - t The string text to tag instead of template tag file.Use quotes to
//          denote string. * overrides - f *
//      -b File is binary
//         not passing this parameter defaults to ASCII files.
//      -a File is ASCII - or not binary.
//      -r Test and read back tags, if any.
//      -o Read back tags, if any, only on one line.
//      
//      Examples:
//      tagexe -t \"SomeCompany, (c) Copyright 2003\" filename.exe
//      Embeds the string given at the end of file name filename.exe.
//      
//      tagexe -f tagfile.txt filename.exe
//      TagEXE will use a pre-defined data file to insert the tag.
//      
//      tagexe -r filename.exe
//      Read back the contents of a tagged file, if present.
//      
//      tagEXE can automatically detect files and change the proper mode for writing.
//      tagEXE supports DOC, JPG, GIF, DLL, EXE, BMP, ZIP, & TXT.
// 
// Compiler: 
//   VS2022 under CMake build. Open the project by opening the parent folder where the code resides.
//   VS2022 Debug command line options -> Right click on CPP or project and choose "Open Debug and Launch settings" and 
//   include "args": [ "/?" ] in the "configurations" section. This is normally called the .\.vs\launch.vs.json file.
//
#include "TagEXE.h"
using namespace std;

int main(int argc, char* argv[])
{
    FILE* fp;

    if (GetCommandLineArgs(argc, argv) ==1)
        return (1);

    //The remaning argument will be destination file.
    sFileName = szInput;

    if (sFileName == NULL)
    {
        printf("\a\n\nNo sourcefilename present. Enter a filename to write to. Use -? for help.\n");
        return -1;
    }

    if (! DetermineFileType(sFileName))
        return (1);

    //Begin! Open the source file, get type too.
    fp = bBinary ? OpenFileAppend(sFileName) : OpenFileAppend(sFileName, false);

    //First, if user wants to read tag, let's get this outta the way.
    if (bReadTag)
    {
        ReadMsgTag(fp);
        DisplayMsgTag();
        return 0;
    }

    //OK, so let's write our message tag to the destination file!
    //If the template text string is empty, then use a template file...
    if (strlen(g_szTemplateText) == 0)
    {
        FILE* hTemplateFile;
        char szTemplateBufferData[256] = { '\0' };

        //TODO: Open read binary by DEFAULT. We should detect or get an option for a binary file.
        hTemplateFile = OpenTemplateFile(g_szTemplateFile);
        strcpy_s(sMsgTagData, ReadMsgTagTemplateFile(hTemplateFile));

        if ((sMsgTagData == NULL) || (strlen(sMsgTagData) == 0))
        {
            //Close our source file
            fclose(fp);
            return (1);
        }
    }
    else
        strcpy_s(sMsgTagData, g_szTemplateText);

    printf("\nTagEXE now working...\n");
    printf("[Source file]: %s\n", sFileName);
    printf("[File Type]  : %s\n", sFileType);
    printf("[Message Tag]: %s\n", sMsgTagData);

    WriteMessageTagContents(fp);
    return (0);
}
