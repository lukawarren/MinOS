#pragma once
#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>

#pragma GCC diagnostic push 
#pragma GCC diagnostic ignored "-Wpedantic"
#include "multiboot.h"
#pragma GCC diagnostic pop

extern "C" void kernel_main(multiboot_info_t* mbd);
void OnCommand(char* buffer);

#endif