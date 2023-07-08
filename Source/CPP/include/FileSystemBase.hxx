#include <vector>

using namespace std;

class FileSystemBase
{
    public:
        FileSystemBase();
        ~FileSystemBase();

        virtual bool Register() = 0;
        virtual vector<const char *> *FileList() = 0;
        virtual vector<const char *> *Contents(const char *) = 0;
};