#include "filesystem/filesystem.h"
#include "filesystem/file.h"
#include "io/uart.h"

namespace Filesystem
{
    static File pFiles[FileDescriptors::N_FILES];

    void Init()
    {
        UART::WriteString("[Filesystem] Built filesystem\n");
    }

    File* GetFile(const FileDescriptor fd)
    {
        assert(fd < FileDescriptors::N_FILES);
        return &pFiles[fd];
    }

}