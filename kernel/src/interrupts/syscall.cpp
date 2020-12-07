#include "syscall.h"
#include "../memory/paging.h"
#include "../memory/idt.h"
#include "../gfx/vga.h"

static void SysPrintf       (Registers syscall);
static void SysThreadExit   (Registers syscall);

static void (*pSyscalls[2])(Registers syscall) =
{
    &SysPrintf,
    &SysThreadExit
};

void HandleSyscalls(Registers syscall)
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

static void SysPrintf(Registers syscall)
{
    // Sanity check address in ecx to check it's within range
    if (!IsPageWithinUserBounds(syscall.ecx)) return;

    VGA_printf((char const*)syscall.ecx);
}

static void SysThreadExit(Registers syscall)
{
    VGA_printf("thread exiting...");
    VGA_printf<uint32_t, true>(syscall.ecx);
}