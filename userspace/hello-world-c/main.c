#include <stdint.h>
#include <stddef.h>

int syscall_args_2(uint32_t id, size_t n1, size_t n2)
{
    int a;
    asm volatile("int $80" : "=a" (a) : "0" (id), "b" (n1), "c" (n2));
    return a;
}

int _start()
{
    char const* message = "Hello from C!";
    syscall_args_2(0, message, 13);
    while (1) {}
    return 0;
}