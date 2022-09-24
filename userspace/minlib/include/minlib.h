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

#ifndef KERNEL

size_t add_messages(struct Message* messages, size_t count)
{
    return __syscall2(SYS_add_messages, (long)messages, (long)count);
}

size_t get_messages(struct Message* messages, size_t count)
{
    return __syscall2(SYS_get_messages, (long)messages, (long)count);
}

#endif

#pragma GCC diagnostic pop
