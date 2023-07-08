#ifndef __FILE_SYSTEM_HXX__
#define __FILE_SYSTEM_HXX__

#include <vector>

using namespace std;

class FileSystem
{
    public:
        FileSystem() = delete;
        ~FileSystem() = delete;

        static bool Register();
        static vector<const char *> *FileList();
        static vector<const char *> *Contents(const char *);
};

#endif // __FILE_SYSTEM_HXX__