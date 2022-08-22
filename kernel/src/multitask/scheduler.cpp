#include "multitask/scheduler.h"
#include "interrupts/pic.h"

namespace multitask
{
    // Assembly variables
    extern "C"
    {
        extern size_t kernel_cr3;
        extern size_t old_stack_address;
        extern size_t old_fxsave_address;
        extern size_t new_stack_address;
        extern size_t new_fxsave_address;
    }

    // Process list
    constexpr size_t max_processes = 128;
    Process processes[max_processes];
    size_t n_processes = 0;

    // External state
    Process* current_process = nullptr;

    void init_scheduler(const size_t _kernel_cr3)
    {
        kernel_cr3 = _kernel_cr3;
    }

    void add_process(Process process)
    {
        processes[n_processes++] = process;
    }

    void on_scheduler()
    {
        assert(n_processes == 1);

        // If we've never been here before, we shouldn't save the old stack
        if (current_process == nullptr)
        {
            old_stack_address = 0;
            old_fxsave_address = 0;
        }
        else
        {
            old_stack_address = (size_t) &processes[0].esp;
            old_fxsave_address = (size_t) &processes[0].fxsave_storage[0];
        }

        new_stack_address = (size_t) &processes[0].esp;
        new_fxsave_address = (size_t) &processes[0].fxsave_storage[0];
        current_process = &processes[0];
        pic::end_interrupt(0);
    }
}