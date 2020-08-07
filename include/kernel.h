#pragma once
#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>

extern "C" void kernel_main(void);
void OnCommand(char* buffer);

#endif