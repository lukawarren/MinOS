#include "syscall.h"
#include "../memory/idt.h"
#include "../gfx/vga.h"

static void SysVgaPrintf    (Syscall syscall);
static void SysThreadExit   (Syscall syscall);

static void (*pSyscalls[2])(Syscall syscall) =
{
    &SysVgaPrintf,
    &SysThreadExit
};

void HandleSyscalls(Syscall syscall)
{
    // Get syscall type
    const uint32_t type = syscall.eax;

    if (type <= (THREAD_EXIT - VGA_PRINTF)) pSyscalls[type](syscall);
    else
    {
        VGA_printf("[Failure] ", false, VGA_COLOUR_LIGHT_RED);
        VGA_printf("Invalid syscall");
    }

    PIC_EndInterrupt(0x80);
}

static void SysVgaPrintf(Syscall syscall)
{
    VGA_printf("printf...");
}

static void SysThreadExit(Syscall syscall)
{
    VGA_printf("thread exiting...");
}