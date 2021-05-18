#include "multitask/syscalls.h"
#include "multitask/multitask.h"
#include "memory/memory.h"
#include "stdout/uart.h"
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

namespace Multitask
{
    static void     _exit(int status); 
    static int      fstat(int file, stat* st);
    static caddr_t  sbrk(int incr);

    int OnSyscall(const Interrupts::StackFrameRegisters sRegisters)
    {
        // Get syscall id
        const uint32_t id = sRegisters.eax;
        UART::WriteString("Syscall ");
        UART::WriteNumber(id);
        UART::WriteString("\n");

        int returnStatus = 0;
        switch (id)
        {
            case 0:
                _exit((int)sRegisters.ebx);
            break;

            case 4:
                returnStatus = fstat((int)sRegisters.ebx, (stat*)sRegisters.ecx);
            break;

            case 12:
                returnStatus = (int) sbrk((int)sRegisters.ebx);
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

    static int fstat(int file __attribute__((unused)), stat* st)
    {
        // TODO: Sanitise memory location
        st->st_mode = S_IFCHR; // Character device
        return 0;
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

}