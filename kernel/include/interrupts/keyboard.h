#pragma once
#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
#include <stddef.h>

void KeyboardInit();
void OnKeyboardInterrupt(uint8_t scancode);

uint32_t GetKeyBufferAddress();

#endif