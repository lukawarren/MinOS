#include "syscall.h"
#include "../file/filesystem.h"
#include "../multitask/multitask.h"
#include "../multitask/elf.h"
#include "../memory/paging.h"
#include "../memory/idt.h"
#include "../gfx/vga.h"
#include "../interrupts/timer.h"
#include "../interrupts/keyboard.h"

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
static int SysGetNextEvent          (Registers syscall);
static int SysPushEvent             (Registers syscall);
static int SysLoadProgram           (Registers syscall);
static int SysSubscribeToStdout     (Registers syscall);
static int SysGetProcess            (Registers syscall);
static int SysSubscribeToSysexit    (Registers syscall);
static int SysGetSeconds            (Registers syscall);
static int SysBlock                 (Registers syscall);
static int SysPopLastEvent          (Registers syscall);
static int SysGetKeyBufferAddr      (Registers syscall);
static int SysSubscribeToKeyboard   (Registers syscall);
static int SysGetSubseconds         (Registers syscall);
static int SysBlockUntil            (Registers syscall);
static int SysKill                  (Registers syscall);

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
    &SysGetNextFile,
    &SysGetNextEvent,
    &SysPushEvent,
    &SysLoadProgram,
    &SysSubscribeToStdout,
    &SysGetProcess,
    &SysSubscribeToSysexit,
    &SysGetSeconds,
    &SysBlock,
    &SysPopLastEvent,
    &SysGetKeyBufferAddr,
    &SysSubscribeToKeyboard,
    &SysGetSubseconds,
    &SysBlockUntil,
    &SysKill
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
    // Sanity check address in ebx to check it's within range
    if (!IsPageWithinUserBounds(syscall.ebx)) return -1;

    OnStdout((char const*)syscall.ebx);

    return 0;
}

static int SysNTasks(Registers syscall __attribute__((unused)))
{
    return (int)GetNumberOfTasks();
}

static int SysSysexit(Registers syscall __attribute__((unused)))
{
    OnSysexit();
    TaskExit();
    return 0;
}

static int SysNPages(Registers syscall __attribute__((unused)))
{
    return (int)GetNumberOfPages();
}

static int SysPrintn (Registers syscall)
{
    OnStdout(syscall.ebx, syscall.ecx);
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

static int SysGetNextEvent(Registers syscall __attribute__((unused)))
{
    return (int)((uint32_t)GetNextEvent());
}

static int SysPushEvent(Registers syscall)
{
    uint32_t processID = syscall.ebx;
    TaskEvent* event = (TaskEvent*)syscall.ecx;
    
    Task* task = GetTaskWithProcessID(processID);
    if (task == nullptr) return -1;

    return PushEvent(task, event);
}

static int SysLoadProgram(Registers syscall)
{
    // Open file
    FileHandle file = kFileOpen((const char*)syscall.ebx);
    if (file == (FileHandle)-1) return -1;

    // Load into memory and parse
    void* fileBuffer = kmalloc(kGetFileSize(file));
    kFileRead(file, fileBuffer);
    auto elf = LoadElfFile(fileBuffer);

    if (elf.error) return -1;

    // Create child task and return process ID
    auto task = CreateChildTask((const char*)syscall.ebx, elf.entry, elf.size, elf.location);
    kfree(fileBuffer, kGetFileSize(file));
    kFileClose(file);

    return (int)task->processID;
}

static int SysSubscribeToStdout(Registers syscall)
{
    SubscribeToStdout(syscall.ebx);
    return 0;
}

static int SysGetProcess(Registers syscall)
{
    const char* process = (const char*)syscall.ebx;

    return (int) GetProcess(process);
}

static int SysSubscribeToSysexit(Registers syscall)
{
    SubscribeToSysexit(syscall.ebx);
    return 0;
}

static int SysGetSeconds(Registers syscall __attribute__((unused)))
{
    return GetSeconds();
}

static int SysBlock(Registers syscall __attribute__((unused)))
{
    OnProcessBlock();
    return 0;
}

static int SysPopLastEvent(Registers syscall)
{
    return (int)PopLastEvent((uint32_t)syscall.ebx);
}

static int SysGetKeyBufferAddr(Registers sycall __attribute__((unused)))
{
    return (int)GetKeyBufferAddress();
}

static int SysSubscribeToKeyboard(Registers syscall)
{
    SubscribeToKeyboard(syscall.ebx);
    return 0;
}

static int SysGetSubseconds(Registers syscall __attribute__((unused)))
{
    return GetSubseconds();
}

static int SysBlockUntil(Registers syscall)
{
    OnProcessBlock((uint32_t) syscall.ebx);
    return 0;
}

static int SysKill(Registers syscall)
{
    uint32_t processID = syscall.ebx;
    Task* task = GetTaskWithProcessID(processID);

    if (task == nullptr) return -1;
    else { OnSysexit(processID); KillTask(task); }
    
    return 0;
}