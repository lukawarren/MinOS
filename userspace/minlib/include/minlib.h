#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#define SYSCALL_NO_TLS 1
#include "../../musl/arch/i386/syscall_arch.h"
#include <unistd.h>

#include "minlib_messages.h"

#define CUSTOM_BIT      (1 << 10)
#define SYS_add_messages CUSTOM_BIT | 1
#define SYS_get_messages CUSTOM_BIT | 2
#define SYS_share_memory CUSTOM_BIT | 3

#ifndef KERNEL

// Custom syscalls

size_t add_messages(struct Message* messages, size_t count)
{
    return __syscall2(SYS_add_messages, (long)messages, (long)count);
}

size_t get_messages(struct Message* messages, size_t count)
{
    return __syscall2(SYS_get_messages, (long)messages, (long)count);
}

int share_memory(size_t address, size_t size, pid_t pid)
{
    return (int)__syscall3(SYS_share_memory, (long)address, (long)size, (long)pid);
}

// C++ userspace

#ifdef __cplusplus
#include <stdlib.h>

extern "C" { int main(); }

void* operator new(size_t size)
{
    void* p = malloc(size);
    return p;
}

void* operator new[](size_t size)
{
    void* p = malloc(size);
    return p;
}

void operator delete(void * p)
{
    free(p);
}

#endif

#endif

#pragma GCC diagnostic pop
