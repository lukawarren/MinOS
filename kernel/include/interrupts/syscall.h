#pragma once
#ifndef SYSCALL_H
#define SYSCALL_H

#include <stddef.h>
#include <stdint.h>
#include "../stdlib.h"

enum Syscalls
{
    VGA_PRINTF,
    THREAD_EXIT
};

extern "C"
{
    void HandleSyscalls(Registers syscall);
}

#endif