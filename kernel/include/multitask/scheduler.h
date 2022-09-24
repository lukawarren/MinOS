#pragma once
#include "klib.h"
#include "multitask/process.h"

namespace multitask
{
    void init_scheduler(const size_t kernel_cr3);
    void add_process(Process process);
    Optional<Process*> get_process(const pid_t pid);

    extern Process* current_process;

    extern "C"
    {
        void on_scheduler();
    }
}