#include <nuttx/config.h>

#include <sys/types.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/boardctl.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>

#include <nuttx/drivers/ramdisk.h>

#include <vector>

#include "romfs_ssemapps.h"
#include "FileSystem.hxx"
#include "Constants.hxx"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Configuration settings */

#ifdef CONFIG_DISABLE_MOUNTPOINT
    #error "Mountpoint support is disabled"
#endif

#ifndef CONFIG_FS_ROMFS
    #error "ROMFS support not enabled"
#endif

#define MOUNT_POINT             "/usr/ssemapps"
#define MOUNT_DEVICE_NAME       "/dev/ram1"
#define RAM_DEVICE_NUMBER       1
#define SECTOR_SIZE             64

/**
 * @brief Calculate the number of sectors required to hold the specified image.
 * 
 * @param imageSize Size of the ROMFS image.
 * @return constexpr int Number of sectors required to hold the image.
 */
constexpr int NumberOfSectors(int imageSize)
{
    return((imageSize + SECTOR_SIZE - 1) / SECTOR_SIZE);
}

/**
 * @brief Register the file system.
 * 
 * @return true If the file system was initialised correctly.
 * @return false If an error occurred.
 */
bool FileSystem::Register()
{
    int ret;
    struct boardioc_romdisk_s desc;

    desc.minor    = RAM_DEVICE_NUMBER;                  /* Minor device number of the ROM disk. */
    desc.nsectors = NumberOfSectors(ssemapps_img_len);  /* The number of sectors in the ROM disk */
    desc.sectsize = SECTOR_SIZE;                        /* The size of one sector in bytes */
    desc.image    = (uint8_t *) ssemapps_img;           /* File system image */

    ret = boardctl(BOARDIOC_ROMDISK, (uintptr_t) &desc);
    if (ret < 0)
    {
        printf("ERROR: Failed to create RAM disk: %s\n", strerror(errno));
        return(false);
    }

    ret = mount(MOUNT_DEVICE_NAME, MOUNT_POINT, "romfs", MS_RDONLY, NULL);
    if (ret < 0)
    {
        printf("ERROR: Mount failed: %s\n", strerror(errno));
        return(false);
    }

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

    DIR *directory = opendir(MOUNT_POINT);
    if (directory != NULL)
    {
        result = new vector<const char *>;
        if (result)
        {
            for (struct dirent *entry = readdir(directory); entry; entry = readdir(directory))
            {
                if (DIRENT_ISFILE(entry->d_type))
                {
                    result->push_back(strdup(entry->d_name));
                }
            }
        }
        closedir(directory);
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

    char fullpath[Constants::MaxPathLength];
    snprintf(fullpath, Constants::LINE_LENGTH, "%s/%s", MOUNT_POINT, filename);
    FILE *file = fopen(fullpath , "r");
    if (file != NULL)
    {
        result = new vector<const char *>;
        if (result)
        {
            char line[Constants::LINE_LENGTH];
            while((fgets(line, Constants::LINE_LENGTH, file) != NULL))
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

    return(result);
}
