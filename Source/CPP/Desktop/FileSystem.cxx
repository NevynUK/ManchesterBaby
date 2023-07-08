#include <sys/stat.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>

#include <vector>

#include "FileSystem.hxx"

using namespace std;

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/**
 * @brief Register the file system.
 * 
 * @return true If the file system was initialised correctly.
 * @return false If an error occurred.
 */
bool FileSystem::Register()
{
    return(true);
}

/**
 * @brief Get a list of files in the specified path.
 * 
 * Note: The caller is responsible for freeing the file name pointers (with free) and the pointer to
 * the vector (with delete) that is returned.
 * 
 * @return vector<const char *>* Pointer to a vector containing a list of files in the directory.
 */
vector<const char *> *FileSystem::FileList()
{
    vector<const char *> *result = nullptr;

    char applicationDirectory[PATH_MAX];
    if (getcwd(applicationDirectory, sizeof(applicationDirectory)) != NULL)
    {
        char ssemApplicationDirectory[PATH_MAX];
        snprintf(ssemApplicationDirectory, PATH_MAX, "%s/../NuttX/SSEMApps", applicationDirectory);
        DIR *directory = opendir(ssemApplicationDirectory);
        if (directory != NULL)
        {
            result = new vector<const char *>;
            if (result)
            {
                for (struct dirent *entry = readdir(directory); entry; entry = readdir(directory))
                {
                    if (entry->d_type == DT_REG)
                    {
                        result->push_back(strdup(entry->d_name));
                    }
                }
            }
        }
    }

    return(result);
}

/**
 * @brief Read the contents of the specified file and return the lines as a vector of lines from the file.
 * 
 * Note that is the responsibility of the caller to dispose of both the lines from the file and the vector
 * returned by this method.
 * 
 * Assumptions:
 *  - The file contains text data only.
 *  - Lines are less than 128 bytes long.
 * 
 * @param filename Name of the file to be read.
 * 
 * @return vector<const char *>* Contents of the file as a vector of const char * pointers.
 */
vector<const char *> *FileSystem::Contents(const char *filename)
{
    vector<const char *> *result = nullptr;

    char applicationDirectory[PATH_MAX];
    if (getcwd(applicationDirectory, sizeof(applicationDirectory)) != NULL)
    {
        char fullpath[PATH_MAX];
        snprintf(fullpath, PATH_MAX, "%s/../NuttX/SSEMApps/%s", applicationDirectory, filename);
        FILE *file = fopen(fullpath , "r");
        if (file != NULL)
        {
            result = new vector<const char *>;
            if (result)
            {
                char line[128];
                while((fgets(line, 128, file) != NULL))
                {
                    char *newline = strchr(line ,'\n');
                    if (newline)
                    {
                        *newline = 0;
                    }
                    result->push_back(strdup(line));
                }
            }
            fclose(file);
        }
    }

    return(result);
}
