#include "../FileSystemBase.hxx"
#include <vector>

using namespace std;

class FileSystem : public FileSystemBase
{
    public:
        FileSystem();
        ~FileSystem();

        bool Register() override;
        vector<const char *> FileList() override;
        vector<const char *> Contents(const char *) override;

    private:
        static const char *MOUNT_POINT;
        static const char *MOUNT_DEVICE_NAME;
        static const int SECTOR_SIZE;
        static const int RAM_DEVICE_NUMBER;

        static constexpr int NumberOfSectors(int);
};