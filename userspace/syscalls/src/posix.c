#include "syscalls.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/times.h>
#include <sys/errno.h>
#include <sys/time.h>
#include <string.h>

#define STDIN  0
#define STDOUT 1
#define STDERR 2

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#define STUB(x) {\
    const char* message = "unmapped POSIX syscall on line " STR(x) "\n";\
    k_error(message, strlen(message));\
    return 0;\
}

#define STUB_HANG(x) {\
    const char* message = "unmapped POSIX syscall on line " STR(x) "\n";\
    k_error(message, strlen(message));\
    while(1) {} \
}

// Processes
void _exit() STUB_HANG(__LINE__)
int kill(int pid, int sig) STUB_HANG(__LINE__)
int getpid() STUB(__LINE__)

// Files
int open(const char *name, int flags, ...) STUB(__LINE__)
int read(int file, char *ptr, int len) STUB(__LINE__)
int write(int file, char *ptr, int len) STUB(__LINE__)
int close(int file) STUB(__LINE__)
int lseek(int file, int ptr, int dir) STUB(__LINE__)
int isatty(int file) STUB(__LINE__)
int fstat(int fd, struct stat* buf)
{
    // Character device for stdout, etc
    if (fd == STDOUT || fd == STDIN || fd == STDERR) buf->st_mode = S_IFCHR;
    else STUB(__LINE__)

    return 0;
}


// Memory
caddr_t sbrk(int incr) STUB(__LINE__)