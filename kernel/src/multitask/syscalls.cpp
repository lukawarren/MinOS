#include "multitask/syscalls.h"
#include "multitask/scheduler.h"
#include "interrupts/pit.h"
#include "memory/memory.h"
#include "fs/fs.h"

namespace multitask
{
    static size_t syscalls[2048] = {};

    // POSIX
    int brk(void *addr);
    uint64_t clock_gettime64(clockid_t clk_id, struct timespec* tp);
    int close(int fd);
    int ioctl(int fd, unsigned long request, char* argp);
    int llseek(unsigned int fd, unsigned long offset_high, unsigned long offset_low, off_t* result, unsigned int whence);
    int madvise(void* addr, size_t length, int advice);
    int mkdir(const char* path, mode_t mode);
    void* mmap(void* addr, size_t length, int prot, int flags, int fd, off_t offset);
    int mprotect(void* addr, size_t length, int prot);
    int munmap(void* addr, size_t len);
    int open(const char* pathname, int flags, mode_t mode);
    ssize_t read(int fd, void* buf, size_t count);
    ssize_t readv(int fd, const struct iovec* iov, int iovcnt);
    int set_thread_area();
    int set_tid_address();
    ssize_t writev(int fd, const iovec* iov, int iovcnt);

    // Custom
    size_t add_messages(Message* messages, size_t count);
    size_t get_messages(Message* messages, size_t count);
    int share_memory(size_t address, size_t size, pid_t pid);

    void init_syscalls()
    {
        // Null out table...
        memset(&syscalls[0], 0, sizeof(syscalls));

        // ...then register what we have
        syscalls[SYS_brk] = (size_t)&brk;
        syscalls[__NR_clock_gettime64] = (size_t)&clock_gettime64;
        syscalls[SYS_close] = (size_t)&close;
        syscalls[SYS_ioctl] = (size_t)&ioctl;
        syscalls[SYS__llseek] = (size_t)&llseek;
        syscalls[SYS_madvise] = (size_t)&madvise;
        syscalls[SYS_mkdir] = (size_t)&mkdir;
        syscalls[SYS_mmap2] = (size_t)&mmap;
        syscalls[SYS_mprotect] = (size_t)&mprotect;
        syscalls[SYS_munmap] = (size_t)&munmap;
        syscalls[SYS_open] = (size_t)&open;
        syscalls[SYS_read] = (size_t)&read;
        syscalls[SYS_readv] = (size_t)&readv;
        syscalls[SYS_set_thread_area] = (size_t)&set_thread_area;
        syscalls[SYS_set_tid_address] = (size_t)&set_tid_address;
        syscalls[SYS_writev] = (size_t)&writev;
        syscalls[SYS_add_messages] = (size_t)&add_messages;
        syscalls[SYS_get_messages] = (size_t)&get_messages;
        syscalls[SYS_share_memory] = (size_t)&share_memory;
    }

    size_t on_syscall(const cpu::Registers registers)
    {
        const size_t id = registers.eax;
        const size_t location = syscalls[id];

        if (location == 0)
        {
            println("Unknown syscall ", id);
            assert(false);
            halt();
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

    template<typename T>
    T* read_from_user(T* address)
    {
        auto& frame = multitask::current_process->frame;
        size_t user_address = frame.virtual_address_to_physical((size_t)address);
        return (T*)user_address;
    }

    int brk(void*)
    {
        // Musl is pretty insistent on using brk instead of mmap but if we
        // just return an error code, it'll give up and use mmap anyway!
        return -1;
    }

    uint64_t clock_gettime64(clockid_t id, struct timespec* tp)
    {
        assert(id == 0);
        auto* user_tp = read_from_user<struct timespec>(tp);
        user_tp->tv_sec = time_t(pit::time_ms / 1000);
        user_tp->tv_nsec = long((pit::time_ms % 1000) * 100000);
        return 0;
    }

    int close(int fd)
    {
        return current_process->close_file(fd) ? 0 : -1;
    }

    int ioctl(int, unsigned long, char*)
    {
        return 0;
    }

    int llseek(unsigned int fd, unsigned long offset_high, unsigned long offset_low, off_t* result, unsigned int whence)
    {
        uint64_t offset = (uint64_t(offset_high) << 32) | offset_low;

        auto seek_result = current_process->seek_file(
            (fs::FileDescriptor)fd,
            offset,
            (Process::SeekMode)whence
        );
        if (!seek_result) return -1;

        *read_from_user<off_t>(result) = (off_t)seek_result.data;
        return 0;
    }

    int madvise(void*, size_t, int)
    {
        // "Advises" the kernel how the memory is expected to be read,
        // allowing read-ahead to be better informed. We don't care
        // about this advice.
        return 0;
    }

    int mkdir(const char*, mode_t)
    {
        return -1;
    }

    // NOTE: the way we call our syscalls means flags (the last argument) wasn't
    //       pushed. Be careful to rectify that should it be needed!
    void* mmap(void* addr, size_t length, int prot, int flags, int fd, off_t)
    {
        // If addr is NULL, then we get to choose :)
        assert(addr == 0);

        // Actual permission of pages - see mprotect for PROT_NONE hack
        assert(prot == (PROT_READ | PROT_WRITE) || prot == PROT_NONE);

        // Check flags - anonymous = no fd, and requires offset be 0
        assert(flags == (MAP_PRIVATE | MAP_ANONYMOUS));
        assert(fd == -1);

        const auto memory = memory::allocate_for_user(length, multitask::current_process->frame);
        return memory.contains_data ? (void*) memory->v_addr : (void*) MAP_FAILED;
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
        // TODO: sanitise (trivial to unmap kernel code for the kernel itself as I'm writing this)
        memory::free_for_user((size_t)addr, len, multitask::current_process->frame);
        return 0;
    }

    int open(const char* pathname, int flags, mode_t)
    {
        assert(flags == O_LARGEFILE);
        const auto result = current_process->open_file(
            read_from_user<const char>(pathname)
        );
        if (result.contains_data) return result.data;
        return -EBADF;
    }

    ssize_t read(int fd, void* buf, size_t count)
    {
        if (!current_process->is_fd_valid(fd)) return -EBADF;
        auto& file = current_process->open_files[fd];

        const auto result = fs::read(
            file.handle,
            read_from_user<void>(buf),
            file.offset,
            count
        );

        if (!result) return -1;

        file.offset += result.data;
        return (ssize_t)result.data;
    }

    ssize_t readv(int fd, const struct iovec* iov, int iovcnt)
    {
        if (!current_process->is_fd_valid(fd)) return -EBADF;
        auto& file = current_process->open_files[fd];

        size_t len = 0;
        for (int i = 0; i < iovcnt; ++i)
        {
            const auto result = fs::read(
                file.handle,
                read_from_user<void>(iov[i].iov_base),
                file.offset,
                iov[i].iov_len
            );

            if (!result) continue;
            len += result.data;
            file.offset += result.data;
        }

        return (ssize_t)len;
    }

    int set_thread_area()
    {
        return 0;
    }

    int set_tid_address()
    {
        return current_process->thread_id;
    }

    ssize_t writev(int fd, const iovec* iov, int iovcnt)
    {
        if (!current_process->is_fd_valid(fd)) return -EBADF;
        auto& file = current_process->open_files[fd];

        size_t len = 0;
        for (int i = 0; i < iovcnt; ++i)
        {
            auto result = fs::write(
                file.handle,
                read_from_user<void>(iov[i].iov_base),
                file.offset,
                iov[i].iov_len
            );

            if (!result) continue;
            len += result.data;
            file.offset += result.data;
        }

        return (ssize_t)len;
    }


    // ---------- Custom syscalls below ---------------


    size_t add_messages(Message* messages, size_t count)
    {
        size_t n_added = 0;

        for (size_t i = 0; i < count; ++i)
        {
            // Use the attached pid to find the target...
            auto process = multitask::get_process(messages[i].pid);
            if (!process) continue;

            // ...then replace it with the sender
            messages[i].pid = current_process->thread_id;
            n_added += (*process)->add_message(
                *read_from_user<Message>(&messages[i])
            );
        }

        return n_added;
    }

    size_t get_messages(Message* messages, size_t count)
    {
        size_t n_gotten = 0;

        for (size_t i = 0; i < count; ++i)
        {
            const auto message = current_process->get_message();

            if (message.contains_data)
            {
                *read_from_user<Message>(&messages[i]) = *message;
                ++n_gotten;
            }
        }

        return n_gotten;
    }

    int share_memory(size_t address, size_t size, pid_t pid)
    {
        // Get process (if any!)
        auto process = multitask::get_process(pid);
        if (!process) return -1;

        // Map...
        using namespace memory;
        const auto v_addr = VirtualAddress(address / PAGE_SIZE) * PAGE_SIZE;
        const auto p_addr = current_process->frame.virtual_address_to_physical(v_addr);
        const auto full_size = PageFrame::round_to_next_page_size(size);

        // ...ensuring all pages are owned by current process
        if (!current_process->frame.owns_memory(v_addr, full_size))
            return -1;

        process.data->frame.map_pages(
            p_addr,
            v_addr,
            USER_PAGE,
            full_size / PAGE_SIZE
        );

        return 0;
    }
}