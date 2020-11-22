#pragma once
#ifndef INTERRUPTS_H
#define INTERRUPS_H

#include <stddef.h>
#include <stdint.h>

#include "../memory/idt.h"
#include "keyboard.h"

void InitInterrupts(uint8_t mask1, uint8_t mask2, Keyboard* k = nullptr);

#endif