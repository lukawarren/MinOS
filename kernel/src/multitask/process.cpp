#include "multitask/process.h"
#include "memory/memory.h"
#include "cpu/gdt.h"
#include "cpu/cpu.h"

namespace multitask
{
    constexpr size_t stack_size = 128 * 1024;
    static pid_t id_pool = 0;

    Process::Process(memory::PageFrame page_frame, const size_t entrypoint) : frame(page_frame)
    {
        // Allocate stack (minus X for alignment) - assume virtual address = physical address as identity mapped
        size_t stack_start_address = memory::allocate_for_user(stack_size, page_frame)->p_addr;
        size_t stack_after_restore = stack_start_address + stack_size - 16;
        size_t* stack = (size_t*) stack_after_restore;

        // Check we'll have 16-byte alignment
        assert(stack_after_restore % 16 == 0);

        // iret frame
        *--stack = 0;                       // stack alignment (if any)
        *--stack = USER_DATA_SEGMENT | 3;   // stack segment (ss)
        *--stack = stack_after_restore;     // esp
        *--stack = 0x202;                   // eflags - default with interrupts on
        *--stack = USER_CODE_SEGMENT | 3;   // cs
        *--stack = entrypoint;              // eip

        // Context switch registers
        *--stack = 0;                       // eax
        *--stack = 0;                       // ecx
        *--stack = 0;                       // edx
        *--stack = 0;                       // ebx
        *--stack = 0;                       // esp
        *--stack = stack_after_restore;     // ebp
        *--stack = 0;                       // esi
        *--stack = 0;                       // edi
        *--stack = USER_DATA_SEGMENT | 3;   // ds
        *--stack = USER_DATA_SEGMENT | 3;   // fs
        *--stack = USER_DATA_SEGMENT | 3;   // es
        *--stack = USER_DATA_SEGMENT | 3;   // gs
        *--stack = page_frame.get_cr3();    // cr3

        // Save esp itself, and other info for syscalls, etc.
        esp = (size_t)stack;
        thread_id = ++id_pool;

        // Create storage for FPU, etc. - must be 16 byte aligned, 512 bytes large
        fxsave_storage = (char*) memory::allocate_for_user(512, page_frame, KERNEL_PAGE)->v_addr;
        assert((size_t)&fxsave_storage[0] % 16 == 0);
        cpu::init_fpu_storage(fxsave_storage);

        // Setup files
        assert(max_files >= 3);
        open_files[fs::get_stdin().fd] = { fs::get_stdin(), 0 };
        open_files[fs::get_stdout().fd] = { fs::get_stdout(), 0 };
        open_files[fs::get_stderr().fd] = { fs::get_stderr(), 0 };
        n_files = 3;
    }

    Optional<fs::FileDescriptor> Process::open_file(const fs::FileHandle handle)
    {
        if (n_files >= max_files) return {};
        open_files[n_files] = { handle, 0 };
        return n_files++;
    }

    Optional<fs::FileDescriptor> Process::open_file(const char* path)
    {
        const auto result = fs::get_file(path);
        if (result.contains_data) return open_file(result.data);
        return {};
    }

    Optional<uint64_t> Process::seek_file(const fs::FileDescriptor fd, const uint64_t offset, const Process::SeekMode mode)
    {
        if (!is_fd_valid(fd)) return {};
        auto& file = open_files[fd];

        switch (mode)
        {
            case SET: file.offset = offset; break;
            case CURRENT: file.offset += offset; break;
            case END: file.offset = file.size() + offset; break;
            default: return {};
        }

        return file.offset;
    }

    bool Process::close_file(const fs::FileDescriptor fd)
    {
        if (!is_fd_valid(fd)) return false;

        // Move all elements above ours downwards by 1, if need be
        for (auto i = fd; i < n_files-1; ++i)
            open_files[i] = open_files[i+1];

        return true;
    }

    bool Process::is_fd_valid(const fs::FileDescriptor fd) const
    {
        return !(fd >= n_files || fd < 0);
    }
}