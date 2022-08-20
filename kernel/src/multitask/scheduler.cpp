#include "multitask/scheduler.h"
#include "interrupts/pic.h"

namespace multitask
{
    // Assembly variables
    extern "C"
    {
        extern size_t kernel_cr3;
        extern size_t new_stack_address;
        extern size_t old_stack_address;
    }

    // House-keeping
    constexpr size_t max_processes = 128;
    Process processes[max_processes];
    size_t n_processes = 0;
    bool left_kernel = false;
    Process* current_process = nullptr;

    void init(const size_t _kernel_cr3)
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

        if (!left_kernel)
        {
            old_stack_address = 0;
            left_kernel = true;
        } else
            old_stack_address = (size_t) &processes[0].esp;

        new_stack_address = (size_t) &processes[0].esp;
        current_process = &processes[0];
        pic::end_interrupt(0);
    }
}