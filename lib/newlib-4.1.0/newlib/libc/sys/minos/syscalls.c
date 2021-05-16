#include <sys/stat.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/times.h>
#include <sys/errno.h>
#include <sys/time.h>
#include <stdio.h>

// "It just works"
#include "../../../../../kernel/include/multitask/syscalls.h"

char **environ; /* pointer to array of char * strings that define the current environment variables */
