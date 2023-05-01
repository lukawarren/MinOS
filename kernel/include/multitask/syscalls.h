#pragma once
#include "cpu/cpu.h"

// We include the Musl headers here so that the "size_t" return type below
// is consistent with that Musl wants (just quietens down my intellisense).
// Such is our reliance upon Musl that userspace must ironically be built
// before the kernel is...
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <sys/uio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <bits/syscall.h>
#include <fcntl.h>

namespace multitask
{
    void init_syscalls();

    extern "C"
    {
        size_t on_syscall(const cpu::Registers registers);
    }
}
