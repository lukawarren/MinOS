#include <features.h>
#include "libc.h"

#define START "_start"

#include "crt_arch.h"

int main();
weak void _init();
weak void _fini();
int __libc_start_main(int (*)(), int, char **,
	void (*)(), void(*)(), void(*)());

/* MinOS */
#include "pthread_impl.h"
struct pthread falseThread;
void* memset(void* ptr, int value, size_t num);
/* MinOS */

void _start_c(long *p)
{
	int argc = p[0];
	char **argv = (void *)(p+1);
	memset(&falseThread, 0, sizeof(struct pthread));
	__libc_start_main(main, argc, argv, _init, _fini, 0);
}
