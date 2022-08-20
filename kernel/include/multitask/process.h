#pragma once
#include "klib.h"
#include "memory/pageFrame.h"
#include <unistd.h>

namespace multitask
{
    class Process
    {
    public:
        Process(memory::PageFrame page_frame, const size_t entrypoint);
        Process() {}
        size_t esp; // Used by context switching
        pid_t thread_id;

    private:
        memory::PageFrame frame;
    };
}