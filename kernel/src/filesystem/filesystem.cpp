#include "filesystem/filesystem.h"
#include "filesystem/file.h"
#include "filesystem/framebufferFile.h"
#include "io/uart.h"

namespace Filesystem
{
    static File pFiles[1];

    void Init()
    {
        pFiles[0] =  FramebufferFile();
        UART::WriteString("[Filesystem] Built filesystem\n");
    }

}