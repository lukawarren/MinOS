#ifdef __cplusplus
#pragma once
#endif
#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <stdint.h>
#include <stddef.h>

#ifndef Kernel

#include <errno.h>
#undef errno
extern int errno;

#include <stdarg.h>

#endif

#ifdef DEFINE_SYSCALLS
#define SYSCALL_ARGS_0(type, fn, num)\
type fn()\
{\
    int a;\
    asm volatile("int $0x80" : "=a" (a) : "0" (num));\
    if (a < 0)\
    {\
        errno = a;\
        return (type) -1;\
    }\
    return (type) a;\
}
#else
#define SYSCALL_ARGS_0(type, fn, num) type fn();
#endif

#ifdef DEFINE_SYSCALLS
#define SYSCALL_ARGS_1(type, fn, num, p1, n1)\
type fn(p1 n1)\
{\
    int a;\
    asm volatile("int $0x80" : "=a" (a) : "0" (num), "b" ((unsigned int)n1));\
    if (a < 0)\
    {\
        errno = a;\
        return (type) -1;\
    }\
    return (type) a;\
}
#else
#define SYSCALL_ARGS_1(type, fn, num, p1, n1) type fn(p1 n1);
#endif

#ifdef DEFINE_SYSCALLS
#define SYSCALL_ARGS_2(type, fn, num, p1, n1, p2, n2)\
type fn(p1 n1, p2 n2)\
{\
    int a;\
    asm volatile("int $0x80" : "=a" (a) : "0" (num), "b" ((unsigned int)n1), "c" ((unsigned int)n2));\
    if (a < 0)\
    {\
        errno = a;\
        return (type) -1;\
    }\
    return (type) a;\
}
#else
#define SYSCALL_ARGS_2(type, fn, num, p1, n1, p2, n2) type fn(p1 n1, p2 n2);
#endif

#ifdef DEFINE_SYSCALLS
#define SYSCALL_ARGS_3(type, fn, num, p1, n1, p2, n2, p3, n3)\
type fn(p1 n1, p2 n2, p3 n3)\
{\
    int a;\
    asm volatile("int $0x80" : "=a" (a) : "0" (num), "b" ((unsigned int)n1), "c" ((unsigned int)n2), "d" ((unsigned int)n3));\
    if (a < 0)\
    {\
        errno = a;\
        return (type) -1;\
    }\
    return (type) a;\
}
#else
#define SYSCALL_ARGS_3(type, fn, num, p1, n1, p2, n2, p3, n3) type fn(p1 n1, p2 n2, p3 n3);
#endif

struct sMmapArgs
{
    void* addr;
    size_t length;
    int prot;
    int flags;
    int fd;
    long int offset;
};

#ifndef Kernel

#ifdef __cplusplus
extern "C" {
#endif

// Posix syscalls

#ifdef DEFINE_SYSCALLS
void _exit(int status)
{
    asm volatile("mov $0, %eax; int $0x80");
}
#else
void _exit(int status);
#endif

SYSCALL_ARGS_1(int, close, 1, int, file)
SYSCALL_ARGS_3(int, execve, 2, const char*, name, char* const*, argv, char* const*, env)
SYSCALL_ARGS_0(int, fork, 3)
SYSCALL_ARGS_2(int, fstat, 4, int, file, struct stat*, st)
SYSCALL_ARGS_0(int, getpid, 5)
SYSCALL_ARGS_1(int, isatty, 6, int, fd)
SYSCALL_ARGS_2(int, kill, 7, int, pid, int, sig)
SYSCALL_ARGS_2(int, link, 8, const char*, old, const char*, _new)
SYSCALL_ARGS_3(off_t, lseek, 9, int, file, off_t, ptr, int, dir)

#ifdef DEFINE_SYSCALLS
int open(const char* name, int flags, ...)
{
    va_list arguments;
    mode_t mode = 0;

    va_start(arguments, mode);
    mode |= va_arg(arguments, mode_t);
    va_end(arguments);

    int a;
    asm volatile("int $0x80" : "=a" (a) : "0" (10), "b" ((unsigned int)name), "c" ((unsigned int)flags), "d" ((uint32_t)mode));
    if (a < 0)
    {
        errno = a;
        return -1;
    }
    return a;
}
#else
int open(const char* name, int flags, ...);
#endif

SYSCALL_ARGS_3(int, read, 11, int, file, void*, ptr, size_t, len)
SYSCALL_ARGS_1(void*, sbrk, 12, ptrdiff_t, incr)
SYSCALL_ARGS_2(int, stat, 13, const char*, file, struct stat*, st)
SYSCALL_ARGS_1(clock_t, times, 14, struct tms*, buf)
SYSCALL_ARGS_1(int, unlink, 15, const char*, name)
SYSCALL_ARGS_1(int, wait, 16, int*, status)
SYSCALL_ARGS_3(int, write, 17, int, file, const void*, ptr, size_t, len)
SYSCALL_ARGS_2(int, gettimeofday, 18, struct timeval*, p, void*, tz)

// MinOS syscalls

#ifdef DEFINE_SYSCALLS
void* mmap(void* addr, size_t length, int prot, int flags, int fd, off_t offset)
{
    volatile struct sMmapArgs args;
    args.addr = addr;
    args.length = length;
    args.prot = prot;
    args.flags = flags;
    args.fd = fd;
    args.offset = offset;

    void* a;
    asm volatile("int $0x80" : "=a" (a) : "0" (19), "b" ((unsigned int)&args));
    return a;
}
#else
void* mmap(void* addr, size_t length, int prot, int flags, int fd, off_t offset);
#endif

SYSCALL_ARGS_2(int, munmap, 20, void*, addr, size_t, length)
SYSCALL_ARGS_3(int, mprotect, 21, void*, addr, size_t, len, int, prot);
SYSCALL_ARGS_0(int, getpagesize, 22)
SYSCALL_ARGS_0(int, getscreenwidth, 23)
SYSCALL_ARGS_0(int, getscreenheight, 24)
SYSCALL_ARGS_0(int, swapscreenbuffer, 25)

#ifdef __cplusplus
}
#endif

#else

#include "interrupts/interrupts.h"

namespace Multitask
{
    extern "C"
    {
        int OnSyscall(const Interrupts::StackFrameRegisters sRegisters);
        extern void IRQ80();
    }
}
#endif

#endif
