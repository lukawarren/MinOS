#ifndef SYSCALLS_H
#define SYSCALL_S

#include <stdint.h>
#include <stddef.h>

#define SYSCALL_ARGS_0(type, fn, num)\
type fn()\
{\
    int a;\
    asm volatile("int $80" : "=a" (a) : "0" (num));\
    return (type) a;\
}

#define SYSCALL_ARGS_1(type, fn, num, p1, n1)\
type fn(p1 n1)\
{\
    int a;\
    asm volatile("int $80" : "=a" (a) : "0" (num), "b" ((unsigned int)n1));\
    return (type) a;\
}

#define SYSCALL_ARGS_2(type, fn, num, p1, n1, p2, n2)\
type fn(p1 n1, p2 n2)\
{\
    int a;\
    asm volatile("int $80" : "=a" (a) : "0" (num), "b" ((unsigned int)n1), "c" ((unsigned int)n2));\
    return (type) a;\
}

SYSCALL_ARGS_2(void, k_error, 0, const char*, message, size_t, len)
SYSCALL_ARGS_2(void, k_print_string, 1, const char*, message, size_t, len)
SYSCALL_ARGS_1(size_t, k_increase_heap, 2, size_t, size)
SYSCALL_ARGS_1(size_t, k_decrease_heap, 3, size_t, size)
SYSCALL_ARGS_0(void, k_exit, 4)

#endif