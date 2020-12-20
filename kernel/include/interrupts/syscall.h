#pragma once
#ifndef SYSCALL_H
#define SYSCALL_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "stdlib.h"

#define SYSCALL_ARGS_0(fn, num) inline int fn() { int a; asm volatile("int $0x80" : "=a" (a) : "0" (num)); return a; }
#define SYSCALL_ARGS_1(fn, num, p1, n1) inline int fn(p1 n1) { int a; asm volatile("int $0x80" : "=a" (a) : "0" (num), "b" ((unsigned int)n1)); return a; }
#define SYSCALL_ARGS_2(fn, num, p1, n1, p2, n2) inline int fn(p1 n1, p2 n2) { int a; asm volatile("int $0x80" : "=a" (a) : "0" (num), "b" ((unsigned int)n1), "c" ((unsigned int)n2)); return a; }

SYSCALL_ARGS_2(printf, 0, char const*, message, bool, newline)
SYSCALL_ARGS_0(nTasks, 1)
SYSCALL_ARGS_0(sysexit, 2)
SYSCALL_ARGS_0(nPages, 3)
SYSCALL_ARGS_2(printn, 4, uint32_t, message, bool, newline)

#ifdef __cplusplus 
extern "C"
{
    int HandleSyscalls(Registers syscall);
}
#endif

#endif