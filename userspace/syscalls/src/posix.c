#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

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
#define IS_STD_OUTPUT(x) (x == STDOUT || x == STDIN || x == STDERR)

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
void _exit()
{
    k_exit();
}

int kill(int pid, int sig) STUB_HANG(__LINE__)
int getpid() STUB(__LINE__)

// Files
int open(const char *name, int flags, ...) STUB(__LINE__)

int read(int fd, char *ptr, int len) STUB(__LINE__)

int write(int fd, char *ptr, int len)
{
    if (IS_STD_OUTPUT(fd)) k_print_string(ptr, len);
    else STUB(__LINE__)
    return len;
}

int close(int fd)
{
    if (!IS_STD_OUTPUT(fd)) STUB(__LINE__)
    return 0;
}

int lseek(int fd, int ptr, int dir) STUB(__LINE__)

int isatty(int fd)
{
    return IS_STD_OUTPUT(fd);
}

int fstat(int fd, struct stat* buf)
{
    // Character device for stdout, etc
    if (IS_STD_OUTPUT(fd)) buf->st_mode = S_IFCHR;
    else STUB(__LINE__)

    return 0;
}

#pragma GCC diagnostic pop