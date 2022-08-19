#pragma once
#include "klib.h"
#include "multitask/process.h"

namespace multitask
{
    void add_process(Process process);

    extern "C"
    {
        void init(const size_t kernel_cr3);
        void on_scheduler();
    }
}