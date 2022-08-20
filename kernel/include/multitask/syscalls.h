#pragma once
#include "cpu/cpu.h"

namespace multitask
{
    extern "C"
    {
        size_t on_syscall(const cpu::Registers registers);
    }
}