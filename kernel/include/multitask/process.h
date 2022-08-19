#pragma once
#include "klib.h"
#include "memory/pageFrame.h"

namespace multitask
{
    class Process
    {
    public:
        Process(memory::PageFrame page_frame, const size_t entrypoint);
        Process() {}
        size_t esp; // Used by context switching

    private:
        memory::PageFrame frame;
    };
}