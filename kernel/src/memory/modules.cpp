#include "memory/modules.h"
#include "stdout/uart.h"

namespace Modules
{
    void Init(const multiboot_info_t* pMultiboot)
    {
        UART::WriteString("[Modules] GRUB modules: ");
        UART::WriteNumber(pMultiboot->mods_count);
        UART::WriteString("\n");
    }
}