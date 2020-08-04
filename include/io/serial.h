#pragma once
#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>

extern "C"
{
    extern void outb(uint16_t port, uint8_t data);
    extern uint8_t inb(uint16_t port);
}

#endif