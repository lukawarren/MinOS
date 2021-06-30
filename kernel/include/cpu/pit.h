#pragma once
#ifndef PIT_H
#define PIT_H

#include <stdint.h>
#include <stddef.h>

#define CHANNEL_0_DATA          0x40
#define CHANNEL_1_DATA          0x41
#define CHANNEL_2_DATA          0x42
#define MODE_COMMAND_REGISTER   0x43

#define MODE_CHANNEL_0          0b00000000
#define MODE_CHANNEL_1          0b01000000
#define MODE_CHANNEL_2          0b10000000

#define MODE_ACCESS_LATCH       0b00000000
#define MODE_ACCESS_LOW_BYTE    0b00010000
#define MODE_ACCESS_HIGH_BYTE   0b00100000
#define MODE_ACCESS_LOW_HIGH    0b00110000

#define MODE_OPERATING_MODE_0   0b00000000
#define MODE_OPERATING_MODE_1   0b00000010
#define MODE_OPERATING_MODE_2   0b00000100
#define MODE_OPERATING_MODE_3   0b00000110
#define MODE_OPERATING_MODE_4   0b00001000
#define MODE_OPERATING_MODE_5   0b00001010

#define MODE_BINARY_16_BIT      0
#define MODE_BINARY_FOUR_DIGIT  1

#define PC_SPEAKER_IO_PORT      0x61
#define PC_SPEAKER_ENABLE       0b11
#define PC_SPEAKER_DISABLE      0xfc

namespace PIT
{
    void Init();
    void SetFrequency(uint16_t channel, uint16_t herz);
    void Reset();
    uint32_t GetDelayInMicroseconds();
}

#endif
