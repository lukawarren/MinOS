#include "pthread_impl.h"
extern struct __pthread falseThread; // defined in crt1.c

static inline uintptr_t __get_tp()
{
	/*
	uintptr_t tp;
	__asm__ ("movl %%gs:0,%0" : "=r" (tp) );
	return tp;
	*/

	struct __pthread* self;
	self = &falseThread;
	return (uintptr_t) self;
}

#define MC_PC gregs[REG_EIP]
