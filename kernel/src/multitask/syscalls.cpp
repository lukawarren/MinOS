#include "multitask/syscalls.h"
#include "multitask/scheduler.h"
#include "io/uart.h"
#include <unistd.h>
#include <sys/uio.h>

namespace multitask
{
    constexpr int fd_stdout = 1;

    int sys_set_thread_area()
    {
        // stub
        return 0;
    }

    int sys_set_tid_address()
    {
        return current_process->thread_id;
    }

    int sys_ioctl(int fd, unsigned long request, char* argp)
    {
        assert(fd == fd_stdout);
        return 0;
    }

    ssize_t sys_writev(int fd, const iovec* iov, int iovcnt)
    {
        assert(fd == fd_stdout);
        assert(iovcnt == 2);
        uart::write_string((const char*)iov[1].iov_base);
        return iov[0].iov_len + iov[1].iov_len;
    }

    size_t on_syscall(const cpu::Registers registers)
    {
        const size_t id = registers.eax;
        int ret = 0;

        switch (id)
        {
            case 54:
                ret = sys_ioctl((int)registers.ebx, (unsigned long)registers.ecx, (char*)registers.edx);
            break;

            case 146:
                ret = sys_writev((int)registers.ebx, (const iovec*)registers.ecx, (int)registers.edx);
            break;

            case 243:
                ret = sys_set_thread_area();
            break;

            case 258:
                ret = sys_set_tid_address();
            break;

            default:
                println("Unknown syscall ", id);
                while(1) {}
            break;
        }

        return (size_t) ret;
    }
}