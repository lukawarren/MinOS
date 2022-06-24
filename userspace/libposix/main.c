#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#define STUB { asm("xchg %bx, %bx"); while(1) {} }

// sys/mman.h
void *mmap(void* addr, size_t length, int prot, int flags, int fd, long int offset) STUB
int munmap(void* addr, size_t len) STUB

// unistd.h
signed int read(int fd, void* buf, size_t count) STUB 
signed int write(int fd, const void *buf, size_t count) STUB

// fcntl.h
int open(const char* pathname, int flags) STUB

// GNU libc
int mtx_lock(void* mutex) STUB
int mtx_unlock(void* mutex) STUB
void mtx_destroy(void* mutex) STUB
void mtx_init(void* mutex, int type) STUB
