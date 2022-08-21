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

        size_t esp;
        pid_t thread_id;
        memory::PageFrame frame;
    };
}