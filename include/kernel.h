#pragma once
#ifndef KERNEL_H
#define KERNEL_H

extern "C" void kernel_main(void);

#include <stdint.h>
void jeff(uint8_t irq);

#endif