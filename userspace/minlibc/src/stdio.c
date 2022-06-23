#include "syscalls.h"

int printf(const char *format, ...)
{
    syscall_args_2(0, format, 128);

    // TODO: return the number of values that are printed
    return 0;
}