#include "multitask/syscalls.h"
#include "multitask/multitask.h"
#include "multitask/mman.h"
#include "memory/memory.h"
#include "io/uart.h"
#include "cpu/pic.h"
#include "kstdlib.h"

#define	S_IFDIR	    0040000 // Directory	
#define	S_IFCHR		0020000 // Character device
#define	S_IFBLK		0060000 // Block device
#define	S_IFREG		0100000 // Regular
#define	S_IFLNK		0120000 // Symbolic link
#define	S_IFSOCK	0140000 // Socket
#define	S_IFIFO		0010000 // FIFO / pipe

struct stat
{
    uint32_t st_dev;     /* ID of device containing file */
    uint16_t st_ino;     /* inode number */
    int32_t  st_mode;    /* protection */
    uint16_t st_nlink;   /* number of hard links */
    uint16_t st_uid;     /* user ID of owner */
    uint16_t st_gid;     /* group ID of owner */
    uint32_t st_rdev;    /* device ID (if special file) */
    long     st_size;    /* total size, in bytes */
    long     st_blksize; /* blocksize for file system I/O */
    long     st_blocks;  /* number of 512B blocks allocated */
    long     st_atime;   /* time of last access */
    long     st_mtime;   /* time of last modification */
    long     st_ctime;   /* time of last status change */
};

typedef	char* caddr_t;

#define STDIN   0
#define STDOUT  1
#define STDERR  2

typedef uint32_t mode_t;

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
    static int      fstat(int fd, stat* st);
    static int      open(const char *pathname, int flags, mode_t mode);
    static caddr_t  sbrk(int incr);
    static int      write(int file, const void* ptr, size_t len);
    static void*    mmap(struct sMmapArgs* args);
    static int      mprotect(void* addr, size_t len, int prot);
    static int      getpagesize();

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
                returnStatus = fstat((int)sRegisters.ebx, (stat*)sRegisters.ecx);
            break;

            case 10:
                returnStatus = open((const char*)sRegisters.ebx, (int)sRegisters.ecx, (uint32_t)sRegisters.edx);
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

            case 21:
                returnStatus = (int)mprotect((void*)sRegisters.ebx, (size_t)sRegisters.ecx, (int)sRegisters.edx);
            break;

            case 22:
                returnStatus = (int) getpagesize();
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
        assert(fd == STDOUT || fd == STDIN || fd == STDERR || fd == 4); // 4 being /dev/fb
        return 0;
    }

    static int fstat(int fd __attribute__((unused)), stat* st)
    {
        // TODO: Sanitise memory location
        assert(fd == STDOUT || fd == STDIN || fd == STDERR);
        st->st_mode = S_IFCHR; // Character device
        return 0;
    }

    static int open(const char *pathname, int flags, mode_t mode __attribute__((unused)))
    {
        auto task = Multitask::GetCurrentTask();
        
        // Get path
        char const* path = (const char*) task->m_PageFrame.VirtualToPhysicalAddress((uint32_t)pathname);
        assert(strcmp(path, "/dev/fb"));

        // Flags
        assert(flags == (FILEIO_O_RDWR | FILEIO_O_CREAT | FILEIO_O_TRUNC));

        // Mode specifies what permissions should be applied, should the file be created

        return 4;  // File descriptor for /dev/fb
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
        assert(file == STDOUT);
        
        // Virtual vs physical address will likely cause us problems
        assert((uint32_t)ptr < USER_PAGING_OFFSET);

        const char* string = (const char*) ptr;
        for (size_t i = 0; i < len; ++i)
            UART::WriteChar(string[i]);

        return 0;
    }

    static void* mmap(struct sMmapArgs* args)
    {
        assert(args->length > 0);

        // TODO: Sanitise memory locations
        Task* task = Multitask::GetCurrentTask();
        
        // Jury rigged for malloc's mmap
        if (args->fd == -1)
        {
            // Prot
            assert(args->prot == PROT_NONE);

            // Flags
            assert(args->flags == (MAP_PRIVATE| MAP_NORESERVE | MAP_ANONYMOUS));

            // File descriptor and offset
            assert(args->offset == 0 && args->fd == -1);

            // Address can be NULL, in which case we're free to do what we like
            assert(args->addr == NULL);

            // Jury rig the flags to avoid mprotect
            return task->m_PageFrame.AllocateMemory(args->length, USER_PAGE);
        }

        // File mmap
        else if (args->fd == 4) // 4 being /dev/fb
        {
            // Surprise - we were already mapped!
            // TODO: Actually map
            return (void*)FRAMEBUFFER_OFFSET;
        }

        // Else time to actually implement mmap!
        else
        {
            assert(false);
            return 0;
        }
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

}