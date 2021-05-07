#pragma once
#ifndef KERNEL_H
#define KERNEL_H

#pragma GCC diagnostic push 
#pragma GCC diagnostic ignored "-Wpedantic"
#include "multiboot.h"
#pragma GCC diagnostic pop

extern "C" void kMain(multiboot_info_t* pMultibootInfo);

#endif