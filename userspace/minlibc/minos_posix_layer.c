#include <threads.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <stdio.h>

#include <syscalls.h>

#define AS_STRING(x) AS_STRING2(x)
#define AS_STRING2(x) #x

#define STUB(x) {\
    const char* m = "unmapped POSIX syscall on line " AS_STRING(x) "\n";\
    size_t l = strlen(m);\
    k_error(m, l);\
}

// sys/mman.h
void *mmap(void* addr, size_t length, int prot, int flags, int fd, long int offset) STUB(__LINE__)
int munmap(void* addr, size_t length) STUB(__LINE__)

// unistd.h
signed int read(int fd, void* buf, size_t count) STUB(__LINE__)

signed int write(int fd, const void *buf, size_t count)
{
    if (fd == 1 || fd == 2) // stdout, stderr
    {
        k_print_string(buf, count);
    }
    else STUB(__LINE__);

    return count;
}

// fcntl.h
int open(const char* pathname, int flags) STUB(__LINE__)

// Thread safety - TODO: implement
int mtx_lock(mtx_t* mutex) { return thrd_success; }
int mtx_unlock(mtx_t* mutex) { return thrd_success; }
void mtx_destroy(mtx_t* mutex) STUB(__LINE__)
int mtx_init(mtx_t* mutex, int type) STUB(__LINE__)
