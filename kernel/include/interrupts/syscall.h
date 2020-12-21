#pragma once
#ifndef SYSCALL_H
#define SYSCALL_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "stdlib.h"

#include "../file/filedefs.h"

#define SYSCALL_ARGS_0(type, fn, num) inline type fn() { int a; asm volatile("int $0x80" : "=a" (a) : "0" (num)); return (type) a; }
#define SYSCALL_ARGS_1(type, fn, num, p1, n1) inline type fn(p1 n1) { int a; asm volatile("int $0x80" : "=a" (a) : "0" (num), "b" ((unsigned int)n1)); return (type) a; }
#define SYSCALL_ARGS_2(type, fn, num, p1, n1, p2, n2) inline type fn(p1 n1, p2 n2) { int a; asm volatile("int $0x80" : "=a" (a) : "0" (num), "b" ((unsigned int)n1), "c" ((unsigned int)n2)); return (type) a; }
#define SYSCALL_ARGS_3(type, fn, num, p1, n1, p2, n2, p3, n3) inline type fn(p1 n1, p2 n2, p3 n3) { int a; asm volatile("int $0x80" : "=a" (a) : "0" (num), "b" ((unsigned int)n1), "c" ((unsigned int)n2), "d" ((unsigned int)n3)); return (type) a; }


SYSCALL_ARGS_2(int, printf, 0, char const*, message, bool, newline)
SYSCALL_ARGS_0(uint32_t, nTasks, 1)
SYSCALL_ARGS_0(int, sysexit, 2)
SYSCALL_ARGS_0(uint32_t, nPages, 3)
SYSCALL_ARGS_2(int, printn, 4, uint32_t, message, bool, newline)
SYSCALL_ARGS_0(uint32_t, getFramebufferAddr, 5)
SYSCALL_ARGS_0(uint32_t, getFramebufferWidth, 6)
SYSCALL_ARGS_0(uint32_t, getFramebufferHeight, 7)
SYSCALL_ARGS_1(void*, malloc, 8, uint32_t, size)
SYSCALL_ARGS_2(int, free, 9, void*, data, uint32_t, size)
SYSCALL_ARGS_1(FileHandle, fileOpen, 10, const char*, sName)
SYSCALL_ARGS_1(uint32_t, getFileSize, 11, FileHandle, file)
SYSCALL_ARGS_2(int, getFileName, 12, FileHandle, file, void*, data)
SYSCALL_ARGS_3(int, fileRead, 13, FileHandle, file, void*, data, uint32_t, size)
SYSCALL_ARGS_1(int, fileClose, 14, FileHandle, file)
SYSCALL_ARGS_1(FileHandle, getNextFile, 15, FileHandle, file)


#ifdef __cplusplus 
extern "C"
{
    int HandleSyscalls(Registers syscall);
}
#endif

#endif