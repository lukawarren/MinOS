#include "multitask/process.h"
#include "memory/memory.h"
#include "cpu/gdt.h"

namespace multitask
{
    constexpr size_t stack_size = 8192;
    static pid_t id_pool = 0;

    Process::Process(memory::PageFrame page_frame, const size_t entrypoint) : frame(page_frame)
    {
        // Allocate stack (minus X for alignment)
        size_t stack_start_address = (size_t) memory::allocate_for_user(stack_size, page_frame);
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
    }
}