#pragma once
#include "klib.h"
#include "memory/page_frame.h"
#include <unistd.h>

namespace multitask
{
    class Process
    {
    public:
        Process(memory::PageFrame page_frame, const size_t entrypoint);
        Process() {}

        // Kernel + syscalls
        memory::PageFrame frame;
        pid_t thread_id;

        // Scheduler variables
        size_t esp;
        char* fxsave_storage; // for fxsave and fxrstor
    };
}