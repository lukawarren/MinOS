#pragma once
#ifndef SYSCALL_H
#define SYSCALL_H

#include <stddef.h>
#include <stdint.h>

struct Syscall
{
    uint32_t esi;
    uint32_t edi;
    uint32_t ebp;
    uint32_t edx;
    uint32_t ecx;
    uint32_t ebx;
    uint32_t eax;
}  __attribute__((packed));

enum Syscalls
{
    VGA_PRINTF,
    THREAD_EXIT
};

extern "C"
{
    void HandleSyscalls(Syscall syscall);
}

#endif