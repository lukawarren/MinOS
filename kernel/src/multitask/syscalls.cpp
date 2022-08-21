#include "multitask/syscalls.h"
#include "multitask/scheduler.h"
#include "fs/fs.h"

namespace multitask
{
    static size_t syscalls[512] = {};

    int ioctl(int fd, unsigned long request, char* argp);
    int set_thread_area();
    int set_tid_address();
    ssize_t writev(int fd, const iovec* iov, int iovcnt);

    void init_syscalls()
    {
        // Null out table...
        memset(&syscalls[0], 0, sizeof(syscalls));

        // ...then register what we have
        syscalls[SYS_ioctl] = (size_t)&ioctl;
        syscalls[SYS_set_thread_area] = (size_t)&set_thread_area;
        syscalls[SYS_set_tid_address] = (size_t)&set_tid_address;
        syscalls[SYS_writev] = (size_t)&writev;
    }

    size_t on_syscall(const cpu::Registers registers)
    {
        const size_t id = registers.eax;
        const size_t location = syscalls[id];

        if (location == 0)
        {
            println("Unknown syscall ", id);
            assert(false);
            while(1) {}
        }

        int ret;
        asm volatile ("\
            pushl %1; \
            pushl %2; \
            pushl %3; \
            pushl %4; \
            pushl %5; \
            call *%6; \
            popl %%ebx; \
            popl %%ebx; \
            popl %%ebx; \
            popl %%ebx; \
            popl %%ebx;" : "=a" (ret) : "r" (registers.edi), "r" (registers.esi),
                            "r" (registers.edx), "r" (registers.ecx), "r" (registers.ebx),
                            "r" (location));

        return (size_t) ret;
    }

    int ioctl(int, unsigned long, char*)
    {
        // stub
        return 0;
    }

    int set_thread_area()
    {
        // stub
        return 0;
    }

    int set_tid_address()
    {
        return current_process->thread_id;
    }

    ssize_t writev(int fd, const iovec* iov, int iovcnt)
    {
        Optional<fs::DeviceFile*> file = fs::get_file(fd);
        if (!file) { return EBADF; }

        ssize_t len = 0;
        for (int i = 0; i < iovcnt; ++i)
            len += (ssize_t) (*(*file)).write(iov[i].iov_base, iov[i].iov_len);

        return len;
    }
}