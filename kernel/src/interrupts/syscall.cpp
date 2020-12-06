#include "syscall.h"
#include "../memory/idt.h"
#include "../gfx/vga.h"

static void SysVgaPrintf    (Registers syscall);
static void SysThreadExit   (Registers syscall);

static void (*pSyscalls[2])(Registers syscall) =
{
    &SysVgaPrintf,
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

static void SysVgaPrintf(Registers syscall)
{
    VGA_printf("printf ", false);
    VGA_printf<uint32_t, true>(syscall.ecx);
}

static void SysThreadExit(Registers syscall)
{
    VGA_printf("thread exiting...");
    VGA_printf<uint32_t, true>(syscall.ecx);
}