#include "syscall.h"
#include "../multitask/multitask.h"
#include "../memory/paging.h"
#include "../memory/idt.h"
#include "../gfx/vga.h"

static int SysPrintf       (Registers syscall);
static int SysGetTasks     (Registers syscall);

static int (*pSyscalls[2])(Registers syscall) =
{
    &SysPrintf,
    &SysGetTasks
};

int HandleSyscalls(Registers syscall)
{
    // Get syscall type
    const uint32_t type = syscall.eax;

    int returnValue = -1;

    if (type <= (THREAD_EXIT - VGA_PRINTF)) returnValue = pSyscalls[type](syscall);
    else
    {
        VGA_printf("[Failure] ", false, VGA_COLOUR_LIGHT_RED);
        VGA_printf("Invalid syscall");
    }

    PIC_EndInterrupt(0x80);
    return returnValue;
}

static int SysPrintf(Registers syscall)
{
    // Sanity check address in ecx to check it's within range
    if (!IsPageWithinUserBounds(syscall.ecx)) return -1;

    VGA_printf((char const*)syscall.ecx);

    return 0;
}

static int SysGetTasks(Registers syscall __attribute__((unused)))
{
    return GetNumberOfTasks();
}