#include "kernel.h"
#include "stdout/uart.h"

extern "C" void kMain(multiboot_info_t* pMultibootInfo)
{
    UART::InitUART();
    UART::WriteString("Hello world!\n");
}