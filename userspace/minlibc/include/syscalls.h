#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <stdint.h>
#include <stddef.h>

int syscall_args_2(uint32_t id, size_t n1, size_t n2);

#endif