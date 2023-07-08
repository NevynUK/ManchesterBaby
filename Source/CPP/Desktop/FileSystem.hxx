#include "FileSystemBase.hxx"
#include <vector>

using namespace std;

class FileSystem : public FileSystemBase
{
    public:
        FileSystem();
        ~FileSystem();

        bool Register() override;
        vector<const char *> *FileList() override;
        vector<const char *> *Contents(const char *) override;
};