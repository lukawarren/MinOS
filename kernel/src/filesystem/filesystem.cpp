#include "filesystem/filesystem.h"
#include "filesystem/file.h"
#include "memory/modules.h"
#include "io/uart.h"

namespace Filesystem
{
    static File pFiles[FileDescriptors::N_FILES];

    struct sCPIOHeader
    {
        char sMagic[6];     // Should be 070707
        char device[6];     // Device
        char ino[6];        // Inode
        char mode[6];       // Mode - permissions and file type
        char uid[6];        // UID
        char gid[6];        // GID
        char nLinks[6];     // Number of links to file
        char rdev[6];       // For block devices and what-not, contains the device number
        char mtime[11];     // Last modified time
        char nameSize[6];   // Size of name string to come
        char fieSize[11];  // File size
    };

    void Init()
    {
        // Load CPIO file
        uint32_t pFile = Modules::GetModule();

        sCPIOHeader* jeff = (sCPIOHeader*)pFile;
        UART::WriteString(jeff->sMagic);

        UART::WriteString("[Filesystem] Built filesystem\n");
    }

    File* GetFile(const FileDescriptor fd)
    {
        assert(fd < FileDescriptors::N_FILES);
        return &pFiles[fd];
    }

}