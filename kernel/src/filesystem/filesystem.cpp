#include "filesystem/filesystem.h"
#include "filesystem/file.h"
#include "filesystem/framebufferFile.h"
#include "io/uart.h"

namespace Filesystem
{
    constexpr uint32_t nFiles = 4; // First 3 are reserved for stdout, stdin and stderr
    static File pFiles[nFiles];

    void Init()
    {
        pFiles[3] =  FramebufferFile();
        UART::WriteString("[Filesystem] Built filesystem\n");
    }

    File GetFile(const FileDescriptor fd)
    {
        assert(fd < nFiles);
        return pFiles[fd];
    }

}