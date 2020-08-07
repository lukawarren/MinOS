#pragma once
#ifndef INTERRUPTS_H
#define INTERRUPS_H

#include <stddef.h>
#include <stdint.h>

#include "../memory/idt.h"
#include "keyboard.h"

void InitInterrupts(Keyboard* k);

#endif