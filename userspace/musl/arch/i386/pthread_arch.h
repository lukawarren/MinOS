#include "syscall.h"
extern struct pthread mainThread; // defined in crt1.c

static inline uintptr_t __get_tp()
{
	/*
	uintptr_t tp;
	__asm__ ("movl %%gs:0,%0" : "=r" (tp) );
	return tp;
	*/

	struct pthread* self;
	self = &mainThread;
	return self;
}

#define MC_PC gregs[REG_EIP]
