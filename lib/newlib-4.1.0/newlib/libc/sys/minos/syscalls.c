#include <sys/stat.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/times.h>
#include <sys/errno.h>
#include <sys/time.h>
#include <stdio.h>

// "It just works"
#define DEFINE_SYSCALLS
#include "../../../../../kernel/include/multitask/syscalls.h"
#include "../../../../../kernel/include/multitask/mman.h"

char **environ; /* pointer to array of char * strings that define the current environment variables */

// Liballoc
static int page_size = -1;

// Need thread safety? Too bad!
int liballoc_lock() { return 0; }
int liballoc_unlock() { return 0; }

void* liballoc_alloc(int pages)
{
    if ( page_size <= 0 ) page_size = getpagesize();
	unsigned int size = pages * page_size;
		
	char *p2 = (char*)mmap(0, size, PROT_NONE, MAP_PRIVATE|MAP_NORESERVE|MAP_ANONYMOUS, -1, 0);
	if ( p2 == MAP_FAILED) return NULL;

	if(mprotect(p2, size, PROT_READ|PROT_WRITE) != 0) 
	{
		munmap(p2, size);
		return NULL;
	}

	return p2;
}

void* _malloc_r (struct _reent *r, size_t n)
{
    malloc(n);
}

void* _free_r (struct _reent *r, size_t n)
{
    free(n);
}

void* _realloc_r (struct _reent *r, size_t n)
{
    realloc(n);
}

void* _calloc_r (struct _reent *r, size_t n)
{
    realloc(n);
}

int liballoc_free(void* ptr, int pages)
{
    return munmap( ptr, pages * page_size );
}
