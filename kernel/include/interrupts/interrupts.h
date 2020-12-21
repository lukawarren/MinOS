#pragma once
#ifndef INTERRUPTS_H
#define INTERRUPS_H

#include <stddef.h>
#include <stdint.h>

#include "../memory/idt.h"

void InitInterrupts(uint8_t mask1, uint8_t mask2);

inline void EnableInterrupts()  { asm volatile("sti"); }
inline void DisableInterrupts() { asm volatile("cli"); }

#endif