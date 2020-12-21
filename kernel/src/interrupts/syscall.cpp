#include "syscall.h"
#include "../multitask/multitask.h"
#include "../memory/paging.h"
#include "../memory/idt.h"
#include "../gfx/vga.h"
#include "../file/filesystem.h"

static int SysPrintf                (Registers syscall);
static int SysNTasks                (Registers syscall);
static int SysSysexit               (Registers syscall);
static int SysNPages                (Registers syscall);
static int SysPrintn                (Registers syscall);
static int SysGetFramebufferAddr    (Registers syscall);
static int SysGetFramebufferWidth   (Registers syscall);
static int SysGetFramebufferHeight  (Registers syscall);
static int SysMalloc                (Registers syscall);
static int SysFree                  (Registers syscall);
static int SysFileOpen              (Registers syscall);
static int SysGetFileSize           (Registers syscall);
static int SysGetFileName           (Registers syscall);
static int SysFileRead              (Registers syscall);
static int SysFileClose             (Registers syscall);
static int SysGetNextFile           (Registers syscall);

static int (*pSyscalls[])(Registers syscall) =
{
    &SysPrintf,
    &SysNTasks,
    &SysSysexit,
    &SysNPages,
    &SysPrintn,
    &SysGetFramebufferAddr,
    &SysGetFramebufferWidth,
    &SysGetFramebufferHeight,
    &SysMalloc,
    &SysFree,
    &SysFileOpen,
    &SysGetFileSize,
    &SysGetFileName,
    &SysFileRead,
    &SysFileClose,
    &SysGetNextFile
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
    return (int)GetNumberOfTasks();
}

static int SysSysexit(Registers syscall __attribute__((unused)))
{
    TaskExit();
    return 0;
}

static int SysNPages(Registers syscall __attribute__((unused)))
{
    return (int)GetNumberOfPages();
}

static int SysPrintn (Registers syscall)
{
    VGA_printf<uint32_t>(syscall.ebx, syscall.ecx);
    return 0;
}

static int SysGetFramebufferAddr(Registers syscall __attribute__((unused))) { return (uint32_t) VGA_framebuffer.address;  }
static int SysGetFramebufferWidth(Registers syscall __attribute__((unused))) { return (uint32_t) VGA_framebuffer.width;   }
static int SysGetFramebufferHeight(Registers syscall __attribute__((unused))) { return (uint32_t) VGA_framebuffer.height; }

static int SysMalloc(Registers syscall) 
{
    auto RoundUpToNextPageSize = [&](uint32_t number)
    {
        int remainder = number % PAGE_SIZE;
        if (remainder == 0)
            return number;
        return number + PAGE_SIZE - remainder;
    };

    // Round to nearest page
    uint32_t size = RoundUpToNextPageSize(syscall.ebx);
    
    // Add to current task's size
    TaskGrow(size);

    return (int) kmalloc(size, USER_PAGE, false);
}

static int SysFree(Registers syscall) 
{ 
    auto RoundUpToNextPageSize = [&](uint32_t number)
    {
        int remainder = number % PAGE_SIZE;
        if (remainder == 0)
            return number;
        return number + PAGE_SIZE - remainder;
    };

    void* data = (void*) syscall.ebx;

    // Round to nearest page
    uint32_t size = RoundUpToNextPageSize(syscall.ecx);
    
    // Add to current task's size
    TaskGrow(-size);

    kfree(data, size);

    return 0;
}

static int SysFileOpen(Registers syscall)
{
    return (int) kFileOpen((const char*)syscall.ebx);
}

static int SysGetFileSize(Registers syscall)
{
    return (int) kGetFileSize((FileHandle)syscall.ebx);
}

static int SysGetFileName(Registers syscall)
{
    char* name = kGetFileName((FileHandle)syscall.ebx);
    strncpy((char*)syscall.ecx, name, MAX_FILE_NAME_LENGTH);
    return 0;
}

static int SysFileRead(Registers syscall)
{
    kFileRead((FileHandle)syscall.ebx, (void*)syscall.ecx, syscall.edx);
    return 0;
}

static int SysFileClose(Registers syscall)
{
    kFileClose((FileHandle)syscall.ebx);
    return 0;
}

static int SysGetNextFile(Registers syscall)
{
    return (int)kGetNextFile((FileHandle)syscall.ebx);
}