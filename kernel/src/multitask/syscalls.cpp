#include "multitask/syscalls.h"
#include "multitask/scheduler.h"
#include "memory/memory.h"
#include "fs/fs.h"

namespace multitask
{
    static size_t syscalls[512] = {};

    int brk(void *addr);
    int ioctl(int fd, unsigned long request, char* argp);
    void* mmap(void* addr, size_t length, int prot, int flags, int fd, off_t offset);
    int mprotect(void* addr, size_t length, int prot);
    int munmap(void* addr, size_t len);
    int set_thread_area();
    int set_tid_address();
    ssize_t writev(int fd, const iovec* iov, int iovcnt);

    void init_syscalls()
    {
        // Null out table...
        memset(&syscalls[0], 0, sizeof(syscalls));

        // ...then register what we have
        syscalls[SYS_brk] = (size_t)&brk;
        syscalls[SYS_ioctl] = (size_t)&ioctl;
        syscalls[SYS_mmap2] = (size_t)&mmap;
        syscalls[SYS_mprotect] = (size_t)&mprotect;
        syscalls[SYS_munmap] = (size_t)&munmap;
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

    int brk(void*)
    {
        // Musl is pretty insistent on using brk instead of mmap, but if we
        // just return an error code, it'll give up and use mmap anyway!
        return -1;
    }

    int ioctl(int, unsigned long, char*)
    {
        // stub
        return 0;
    }

    void* mmap(void* addr, size_t length, int prot, int flags, int fd, off_t offset)
    {
        // If addr is NULL, then we get to choose :)
        assert(addr == 0);

        // Actual permission of pages - see mprotect for PROT_NONE hack
        assert(prot == (PROT_READ | PROT_WRITE) || prot == PROT_NONE);

        // Check flags - anonymous = no fd, and requires offset be 0
        assert(flags == (MAP_PRIVATE | MAP_ANONYMOUS));
        assert(fd == -1);
        assert(offset == 0);

        auto memory = memory::allocate_for_user(length, multitask::current_process->frame);
        return memory.contains_data ? (void*) *memory : (void*) MAP_FAILED;
    }

    int mprotect(void*, size_t, int prot)
    {
        // Musl likes to allocate pages ahead of time with PROT_NONE, then "enable"
        // them later here. That's a bit too much effort right now, so instead we
        // always map them as READ | WRITE, meaning we've nothing to do here.
        assert(prot == (PROT_READ | PROT_WRITE));
        return 0;
    }

    int munmap(void* addr, size_t len)
    {
        memory::free_for_user((size_t)addr, len, multitask::current_process->frame);
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
        {
            auto& frame = multitask::current_process->frame;
            size_t address = frame.virtual_address_to_physical((size_t) iov[i].iov_base);
            len += (ssize_t) (*(*file)).write((void*)address, iov[i].iov_len);
        }

        return len;
    }
}