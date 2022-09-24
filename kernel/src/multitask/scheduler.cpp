#include "multitask/scheduler.h"
#include "interrupts/pic.h"
#include "interrupts/pit.h"

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
    size_t n_current_process = 0;

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

    Optional<Process*> get_process(const pid_t pid)
    {
        for (size_t i = 0; i < n_processes; ++i)
            if (processes[i].thread_id == pid)
                return &processes[i];

        return {};
    }

    void on_scheduler()
    {
        assert(n_processes > 0);

        if (current_process == nullptr)
        {
            // If we've never been here before, we shouldn't save the old stack
            old_stack_address = 0;
            old_fxsave_address = 0;
        }
        else
        {
            // Preserve old process
            old_stack_address = (size_t) &processes[n_current_process].esp;
            old_fxsave_address = (size_t) &processes[n_current_process].fxsave_storage[0];

            // Pick new process; round robin
            ++n_current_process %= n_processes;
        }

        // Load new process
        new_stack_address = (size_t) &processes[n_current_process].esp;
        new_fxsave_address = (size_t) &processes[n_current_process].fxsave_storage[0];
        current_process = &processes[n_current_process];

        pit::reload();
        pic::end_interrupt(0);
    }
}