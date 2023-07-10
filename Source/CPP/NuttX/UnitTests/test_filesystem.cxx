#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "FileSystem.hxx"

#define FILE_TO_READ        "hfr989.ssem"

static const char *hfr989ExpectedContents[] = 
{
    "01:   LDN 18",
    "02:   LDN 19",
    "03:   SUB 20",
    "04:   CMP",
    "05:   JRP 21",
    "06:   SUB 22",
    "07:   STO 24",
    "08:   LDN 22",
    "09:   SUB 23",
    "10:   STO 20",
    "11:   LDN 20",
    "12:   STO 22",
    "13:   LDN 24",
    "14:   CMP",
    "15:   JMP 25",
    "16:   JMP 18",
    "17:   STOP",
    "18:   NUM 0",
    "19:   NUM -989",
    "20:   NUM 988",
    "21:   NUM -3",
    "22:   NUM -988",
    "23:   NUM 1",
    "24:   NUM 0",
    "25:   NUM 16",
};

bool TestFileSystem()
{
    if (!FileSystem::Register())
    {
        printf("Error initialising the file system.\n");
        return(false);
    }

    vector<const char *> *fileList = FileSystem::FileList();
    if (!fileList)
    {
        printf("Cannot get list of files.\n");
        return(false);
    }
    bool found = false;
    for (auto fileName : *fileList)
    {
        if (strcmp(fileName, FILE_TO_READ) == 0)
        {
            found = true;
        }
        free((void *) fileName);
    }
    fileList->clear();
    if (!found)
    {
        printf("Cannot find " FILE_TO_READ " on file store.\n");
        return(false);
    }
    delete fileList;
    
    vector<const char *> *contents = FileSystem::Contents(FILE_TO_READ);
    if (!contents)
    {
        printf("Cannot read file " FILE_TO_READ "\n");
        return(false);
    }
    int lineNumber = 0;
    for (auto line : *contents)
    {
        if (strcmp(hfr989ExpectedContents[lineNumber], line) != 0)
        {
            printf("Failed comparing line number %d, expected '%s', read '%s'\n", lineNumber, hfr989ExpectedContents[lineNumber], line);
            return(false);
        }
        lineNumber++;
        free((void *) line);
    }
    contents->clear();
    delete contents;

    return(true);
}