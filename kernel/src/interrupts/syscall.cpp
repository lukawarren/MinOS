#include "syscall.h"
#include "../multitask/multitask.h"
#include "../memory/paging.h"
#include "../memory/idt.h"
#include "../gfx/vga.h"

static int SysPrintf                (Registers syscall);
static int SysNTasks                (Registers syscall);
static int SysSysexit               (Registers syscall);
static int SysNPages                (Registers syscall);
static int SysPrintn                (Registers syscall);
static int SysGetFramebufferAddr    (Registers syscall);
static int SysGetFramebufferWidth   (Registers syscall);
static int SysGetFramebufferHeight  (Registers syscall);

static int (*pSyscalls[])(Registers syscall) =
{
    &SysPrintf,
    &SysNTasks,
    &SysSysexit,
    &SysNPages,
    &SysPrintn,
    &SysGetFramebufferAddr,
    &SysGetFramebufferWidth,
    &SysGetFramebufferHeight
};

int HandleSyscalls(Registers syscall)
{
    // Get syscall type
    const uint32_t type = syscall.eax;

    int returnValue = -1;
    if (type < sizeof(pSyscalls) / sizeof(pSyscalls[0])) returnValue = pSyscalls[type](syscall);
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
    if (!IsPageWithinUserBounds(syscall.ebx)) return -1;

    VGA_printf((char const*)syscall.ebx, syscall.ecx);

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

static int SysNPages(Registers syscall __attribute__((unused)))
{
    return GetNumberOfPages();
}

static int SysPrintn (Registers syscall)
{
    VGA_printf<uint32_t>(syscall.ebx, syscall.ecx);
    return 0;
}

static int SysGetFramebufferAddr(Registers syscall __attribute__((unused))) { return (uint32_t) VGA_framebuffer.address;  }
static int SysGetFramebufferWidth(Registers syscall __attribute__((unused))) { return (uint32_t) VGA_framebuffer.width;   }
static int SysGetFramebufferHeight(Registers syscall __attribute__((unused))) { return (uint32_t) VGA_framebuffer.height; }