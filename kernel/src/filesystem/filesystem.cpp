#include "filesystem/filesystem.h"
#include "filesystem/file.h"
#include "filesystem/framebufferFile.h"
#include "io/uart.h"

namespace Filesystem
{
    constexpr uint32_t nFiles = 1;
    static File pFiles[nFiles];

    void Init()
    {
        pFiles[0] =  FramebufferFile();
        UART::WriteString("[Filesystem] Built filesystem\n");
    }

    File GetFile(const FileDescriptor fd)
    {
        assert(fd < nFiles);
        return pFiles[fd];
    }

}