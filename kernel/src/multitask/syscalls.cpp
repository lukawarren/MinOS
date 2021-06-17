#include "multitask/syscalls.h"
#include "multitask/multitask.h"
#include "filesystem/filesystem.h"
#include "multitask/mman.h"
#include "memory/memory.h"
#include "io/uart.h"
#include "io/gfx/framebuffer.h"
#include "cpu/pic.h"
#include "kstdlib.h"

#define	S_IFDIR	    0040000 // Directory	
#define	S_IFCHR		0020000 // Character device
#define	S_IFBLK		0060000 // Block device
#define	S_IFREG		0100000 // Regular
#define	S_IFLNK		0120000 // Symbolic link
#define	S_IFSOCK	0140000 // Socket
#define	S_IFIFO		0010000 // FIFO / pipe

typedef uint32_t uid_t;
typedef uint32_t gid_t;
typedef int pid_t;
typedef char* caddr_t;
typedef int id_t;
typedef uint32_t ino_t;
typedef int64_t off_t;
typedef uint32_t blkcnt_t;
typedef uint32_t blksize_t;
typedef uint32_t dev_t;
typedef uint16_t mode_t;
typedef uint32_t nlink_t;
typedef int64_t time_t;
typedef uint32_t useconds_t;
typedef int32_t suseconds_t;
typedef uint32_t clock_t;

struct stat
{
    dev_t     st_dev;     /* ID of device containing file */
    ino_t     st_ino;     /* inode number */
    mode_t    st_mode;    /* protection */
    nlink_t   st_nlink;   /* number of hard links */
    uid_t     st_uid;     /* user ID of owner */
    gid_t     st_gid;     /* group ID of owner */
    dev_t     st_rdev;    /* device ID (if special file) */
    off_t     st_size;    /* total size, in bytes */
    blksize_t st_blksize; /* blocksize for file system I/O */
    blkcnt_t  st_blocks;  /* number of 512B blocks allocated */
    time_t    st_atime;   /* time of last access */
    time_t    st_mtime;   /* time of last modification */
    time_t    st_ctime;   /* time of last status change */
};

#define STDIN   0
#define STDOUT  1
#define STDERR  2

// Open flags
#define FILEIO_O_RDONLY           0x0
#define FILEIO_O_WRONLY           0x1
#define FILEIO_O_RDWR             0x2
#define FILEIO_O_APPEND           0x8
#define FILEIO_O_CREAT          0x200
#define FILEIO_O_TRUNC          0x400
#define FILEIO_O_EXCL           0x800
#define FILEIO_O_SUPPORTED	(FILEIO_O_RDONLY | FILEIO_O_WRONLY| \
				 FILEIO_O_RDWR   | FILEIO_O_APPEND| \
				 FILEIO_O_CREAT  | FILEIO_O_TRUNC| \
				 FILEIO_O_EXCL)

// mode_t bits
#define FILEIO_S_IFREG        0100000
#define FILEIO_S_IFDIR         040000
#define FILEIO_S_IFCHR         020000
#define FILEIO_S_IRUSR           0400   // User has read permission
#define FILEIO_S_IWUSR           0200
#define FILEIO_S_IXUSR           0100
#define FILEIO_S_IRWXU           0700
#define FILEIO_S_IRGRP            040
#define FILEIO_S_IWGRP            020   // Group has write permission
#define FILEIO_S_IXGRP            010   // Group has execute permission
#define FILEIO_S_IRWXG            070
#define FILEIO_S_IROTH             04
#define FILEIO_S_IWOTH             02
#define FILEIO_S_IXOTH             01
#define FILEIO_S_IRWXO             07
#define FILEIO_S_SUPPORTED         (FILEIO_S_IFREG|FILEIO_S_IFDIR|  \
				    FILEIO_S_IRWXU|FILEIO_S_IRWXG|  \
                                    FILEIO_S_IRWXO)

namespace Multitask
{
    static void     _exit(int status);
    static int      close(int fd);
    static int      fstat(int fd, struct stat* st);
    static int      getpid();
    static int      isatty(int fd);
    static int      kill(int pid, int sig);
    static int      open(const char *pathname, int flags, mode_t mode);
    static caddr_t  sbrk(int incr);
    static int      write(int file, const void* ptr, size_t len);
    static void*    mmap(struct sMmapArgs* args);
    static int      munmap(void* addr, size_t length);
    static int      mprotect(void* addr, size_t len, int prot);
    static int      getpagesize();
    static int      getscreenwidth();
    static int      getscreenheight();
    static int      swapscreenbuffer();
    //static int      block();
    static int      sendmessage(Message* message, int pid);
    static int      getmessage(Message* message);

    int OnSyscall(const Interrupts::StackFrameRegisters sRegisters)
    {
        // Get syscall id
        const uint32_t id = sRegisters.eax;

        int returnStatus = 0;
        switch (id)
        {
            case 0:
                _exit((int)sRegisters.ebx);
            break;

            case 1:
                returnStatus = close((int)sRegisters.ebx);
            break;

            case 4:
                returnStatus = fstat((int)sRegisters.ebx, (struct stat*)sRegisters.ecx);
            break;

            case 5:
                returnStatus = getpid();
            break;

            case 6:
                returnStatus = isatty((int)sRegisters.ebx);
            break;

            case 7:
                returnStatus = kill((int)sRegisters.ebx, (int)sRegisters.ecx);
            break;

            case 10:
                returnStatus = open((const char*)sRegisters.ebx, (int)sRegisters.ecx, (mode_t)sRegisters.edx);
            break;

            case 12:
                returnStatus = (int) sbrk((int)sRegisters.ebx);
            break;

            case 17:
                returnStatus = write((int)sRegisters.ebx, (const void*)sRegisters.ecx, (size_t)sRegisters.edx);
            break;

            case 19:
                returnStatus = (int) mmap((struct sMmapArgs*)sRegisters.ebx);
            break;

            case 20:
                returnStatus = (int) munmap((void*)sRegisters.ebx, (size_t)sRegisters.ecx);
            break;

            case 21:
                returnStatus = (int)mprotect((void*)sRegisters.ebx, (size_t)sRegisters.ecx, (int)sRegisters.edx);
            break;

            case 22:
                returnStatus = (int) getpagesize();
            break;

            case 23:
                returnStatus = getscreenwidth();
            break;

            case 24:
                returnStatus = getscreenheight();
            break;

            case 25:
                returnStatus = swapscreenbuffer();
            break;

            case 26:
                //returnStatus = block();
            break;
            
            case 27:
                returnStatus = sendmessage((Message*)sRegisters.ebx, (int)sRegisters.ecx);
            break;
            
            case 28:
                returnStatus = getmessage((Message*)sRegisters.ebx);
            break;

            default:
                UART::WriteString("[Syscall] Unexpected syscall ");
                UART::WriteNumber(id);
                UART::WriteString(" by task ");
                UART::WriteString(Multitask::GetCurrentTask()->m_sName);
                UART::WriteString("\n");
                
                RemoveCurrentTask();
                OnTaskSwitch(false);
                Interrupts::bSwitchTasks = true;
            break;
        }

        PIC::EndInterrupt(0x80);
        return returnStatus;
    }

    static void _exit(int status)
    {
        UART::WriteString("[Syscall] Task ");
        UART::WriteString(GetCurrentTask()->m_sName);
        UART::WriteString(" exited with code ");
        UART::WriteNumber(status);
        UART::WriteString("\n");

        RemoveCurrentTask();
        OnTaskSwitch(false);
        Interrupts::bSwitchTasks = true;
    }

    static int close(int fd)
    {
        assert(fd == STDOUT || fd == STDIN || fd == STDERR || fd == Filesystem::FileDescriptors::framebuffer || fd == Filesystem::FileDescriptors::mouse);
        return 0;
    }

    static int fstat(int fd, struct stat* st)
    {
        // TODO: Sanitise memory location
        struct stat* kernelMappedSt = (struct stat*) Multitask::GetCurrentTask()->m_PageFrame.VirtualToPhysicalAddress((uint32_t)st);

        if(fd == STDOUT || fd == STDIN || fd == STDERR) kernelMappedSt->st_mode = S_IFCHR; // Character device
        else
        {
            // Block device
            kernelMappedSt->st_mode = S_IFBLK;
            kernelMappedSt->st_size = Filesystem::GetFile(fd)->m_Size;
        }

        return 0;
    }

    static int getpid()
    {
        return Multitask::GetCurrentTask()->m_PID;
    }

    static int isatty(int fd)
    {
        return
        (fd == Filesystem::FileDescriptors::stderr  ) ||
        (fd == Filesystem::FileDescriptors::stdin   ) ||
        (fd == Filesystem::FileDescriptors::stdout  );
    }

    static int kill(int pid, int sig)
    {
        UART::WriteString("[Syscall] Task ");
        UART::WriteString(GetCurrentTask()->m_sName);
        UART::WriteString(" killed task ");
        UART::WriteNumber(pid);
        UART::WriteString(" with status ");
        UART::WriteNumber(sig);
        UART::WriteString("\n");

        Multitask::RemoveTaskWithID(pid);
        return 0;
    }

    static int open(const char *pathname, int flags, mode_t mode __attribute__((unused)))
    {
        auto task = Multitask::GetCurrentTask();
        
        // Get path
        char const* path = (const char*) task->m_PageFrame.VirtualToPhysicalAddress((uint32_t)pathname);

        // Flags
        assert(flags == (FILEIO_O_RDWR | FILEIO_O_CREAT | FILEIO_O_TRUNC));

        // (Mode specifies what permissions should be applied, should the file be created)

        return Filesystem::GetFile(path)->m_iNode;
    }

    static caddr_t sbrk(int incr)
    {
        assert(incr > 0 && incr != 0);

        // Allocate memory to process - sbrk is sort of a relic - normally this memory has to be contiguous!
        // Mac OSX solves this by giving a fixed 4mb pool for each process's sbrk calls, and crashing after that,
        // so that's what I do too (well I don't know about the 4mb but you get the idea).
        auto task = Multitask::GetCurrentTask();
        
        if (task->m_pSbrkBuffer == nullptr)
        {
            task->m_PageFrame.AllocateMemory(SBRK_BUFFER_MAX_SIZE, USER_PAGE, 0x50000000);
        }

        const uint32_t oldAddress = (uint32_t)task->m_pSbrkBuffer + task->m_nSbrkBytesUsed;
        task->m_nSbrkBytesUsed += (uint32_t) incr;

        // If memory has overflown, kill the process
        if (task->m_nSbrkBytesUsed >= SBRK_BUFFER_MAX_SIZE)
        {
            UART::WriteString("[Syscall] Task ");
            UART::WriteString(GetCurrentTask()->m_sName);
            UART::WriteString(" filled sbrk buffer\n");
            UART::WriteString("\n");
            RemoveCurrentTask();
            OnTaskSwitch(false);
            Interrupts::bSwitchTasks = true;
        }

        return (caddr_t) oldAddress;
    }

    static int write(int file, const void* ptr, size_t len)
    {
        // TODO: Sanitise memory locations
        assert(file == STDOUT || file == STDERR);

        // Get string
        const auto task = Multitask::GetCurrentTask();
        const char* string = (const char*) task->m_PageFrame.VirtualToPhysicalAddress((uint32_t)ptr);

        // Print
        for (size_t i = 0; i < len; ++i)
            UART::WriteChar(string[i]);

        return 0;
    }

    static void* mmap(struct sMmapArgs* args)
    {
        // TODO: Sanitise memory locations
        Task* task = Multitask::GetCurrentTask();
        struct sMmapArgs* kArgs = (struct sMmapArgs*) task->m_PageFrame.VirtualToPhysicalAddress((uint32_t)args);

        assert(kArgs->length > 0);

        // Jury rigged for malloc's mmap
        if (kArgs->fd == -1)
        {
            // Prot
            assert(kArgs->prot == PROT_NONE);

            // Flags
            assert(kArgs->flags == (MAP_PRIVATE| MAP_NORESERVE | MAP_ANONYMOUS));

            // File descriptor and offset
            assert(kArgs->offset == 0 && kArgs->fd == -1);

            // Address can be NULL, in which case we're free to do what we like
            assert(kArgs->addr == NULL);

            // Jury rig the flags to avoid mprotect
            return task->m_PageFrame.AllocateMemory(kArgs->length, USER_PAGE);
        }

        // File mmap
        else
        {
            assert(kArgs->offset == 0);
            assert(kArgs->addr == NULL);

            // (ignore flags, protection, length, and everything else for now)

            Filesystem::File* file = Filesystem::GetFile(kArgs->fd);
            
            // Map into memory
            uint32_t address = (uint32_t) task->m_PageFrame.AllocateMemory(file->m_Size, USER_PAGE);
            for (uint32_t i = 0; i < Memory::RoundToNextPageSize(file->m_Size) / PAGE_SIZE; ++i)
            {
                // Last argument tells page frame to not reflect mapping in bitmap as pages are already marked as used in kernel
                task->m_PageFrame.SetPage((uint32_t)file->m_pData + i*PAGE_SIZE, address + i*PAGE_SIZE, USER_PAGE, false);
            }

            return (void*)address;
        }
    }

    static int munmap(void* addr, size_t length)
    {
        Task* task = Multitask::GetCurrentTask();

        // TODO: Decide if to reflect in kernel? Seems to work but you never know!
        const uint32_t virtualAddress = (uint32_t) addr;
        const uint32_t physicalAddress = task->m_PageFrame.VirtualToPhysicalAddress(virtualAddress);

        task->m_PageFrame.FreeMemory(physicalAddress, virtualAddress, length);
        return 0;
    }

    static int mprotect(void* addr __attribute__((unused)), size_t len __attribute__((unused)), int prot __attribute__((unused)))
    {
        /*
            // TODO: Sanitise memory locations
            auto task = Multitask::GetCurrentTask();
            
            // Virtual vs physical address will likely cause us problems
            assert((uint32_t)addr < USER_PAGING_OFFSET);

            // Check alignment and what-not
            assert((uint32_t)addr % PAGE_SIZE == 0 && len % PAGE_SIZE == 0);

            // Flags
            assert(prot == (PROT_READ | PROT_WRITE));

            for (int i = 0; i < len / PAGE_SIZE; ++i)
                task->m_PageFrame.SetPage((uint32_t)addr + i*PAGE_SIZE, (uint32_t)addr + i*PAGE_SIZE, USER_PAGE);
        */

        // See mmap, we've jury rigged it!
        return 0;
    }

    static int getpagesize()
    {
        return PAGE_SIZE;
    }

    static int getscreenwidth()
    {
        return Framebuffer::graphicsDevice->m_Width;
    }
    
    static int getscreenheight()
    {
        return Framebuffer::graphicsDevice->m_Height;
    }

    static int swapscreenbuffer()
    {
        Framebuffer::graphicsDevice->SwapBuffers();
        return 0;
    }
    
    static int sendmessage(Message* message, int pid)
    {
        Task* task = Multitask::GetCurrentTask();
        Message* pMessage = (Message*) task->m_PageFrame.VirtualToPhysicalAddress((uint32_t)message);
        
        Task* destTask = Multitask::GetTaskWithID(pid);
        assert(destTask != nullptr);
        destTask->AddMesage(task->m_PID, pMessage->data);
        
        return 0;
    }
    
    static int getmessage(Message* message)
    {
        Task* task = Multitask::GetCurrentTask();
        Message* pMessage = (Message*) task->m_PageFrame.VirtualToPhysicalAddress((uint32_t)message);
        
        if (task->m_nMessages)
        {
            Multitask::GetCurrentTask()->GetMessage(pMessage);
            return 1;
        }
        
        return 0;
    }

}
