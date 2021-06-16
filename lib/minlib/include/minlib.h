#pragma once
#ifndef MINLIB_H
#define MINLIB_H

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/stat.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wredundant-decls"
#include "../../../kernel/include/multitask/syscalls.h"
#include "../../../kernel/include/multitask/mman.h"
#pragma GCC diagnostic pop

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

extern void* operator new(size_t size);
extern void operator delete(void * p);

int main();

#endif
