#ifndef __FILE_SYSTEM_HXX__
#define __FILE_SYSTEM_HXX__

#include <vector>

using namespace std;

class FileSystem
{
    public:
        /**
         * @brief Default constructor is not used in this application.
         */
        FileSystem() = delete;

        /**
         * @brief Default destructor is not used in this application.
         */
        ~FileSystem() = delete;

        /**
         * @brief Register the file system.
         * 
         * @return true If the file system was initialised correctly.
         * @return false If an error occurred.
         */
        static bool Register();

        /**
         * @brief Get a list of files in the specified path.
         * 
         * Note: The caller is responsible for freeing the file name pointers (with free) and the pointer to
         * the vector (with delete) that is returned.
         * 
         * @return vector<const char *>* Pointer to a vector containing a list of files in the directory.
         */
        static vector<const char *> *FileList();

        /**
         * @brief Read the contents of the specified file and return the lines as a vector of lines from the file.
         * 
         * Note that is the responsibility of the caller to dispose of both the lines from the file and the vector
         * returned by this method.
         * 
         * Assumptions:
         *  - The file contains text data only.
         *  - Lines are less than Constants::LINE_LENGTH bytes long.
         * 
         * @param filename Name of the file to be read.
         * 
         * @return vector<const char *>* Contents of the file as a vector of const char * pointers.
         */
        static vector<const char *> *Contents(const char *);
};

#endif // __FILE_SYSTEM_HXX__