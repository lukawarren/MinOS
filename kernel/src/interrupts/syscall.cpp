#include "syscall.h"
#include "../multitask/multitask.h"
#include "../memory/paging.h"
#include "../memory/idt.h"
#include "../gfx/vga.h"

enum Syscalls
{
    VGA_PRINTF,
    NTASKS,
    SYSEXIT
};

static int SysPrintf        (Registers syscall);
static int SysNTasks        (Registers syscall);
static int SysSysexit       (Registers syscall);

static int (*pSyscalls[3])(Registers syscall) =
{
    &SysPrintf,
    &SysNTasks,
    &SysSysexit
};

int HandleSyscalls(Registers syscall)
{
    // Get syscall type
    const uint32_t type = syscall.eax;

    int returnValue = -1;

    if (type <= (SYSEXIT - VGA_PRINTF)) returnValue = pSyscalls[type](syscall);
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

static int SysNTasks(Registers syscall __attribute__((unused)))
{
    return GetNumberOfTasks();
}

static int SysSysexit(Registers syscall __attribute__((unused)))
{
    TaskExit();
    return 0;
}