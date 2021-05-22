#pragma once
#ifndef MOUSE_H
#define MOUSE_H

#include <stdint.h>
#include <stddef.h>

/*

    IO PORT     Access Type     Purpose
    0x60        Read/Write      Data
    0x64        Read            Status
    0x64        Write           Command
*/

#define PS2_DATA_PORT 0x60
#define PS2_STATUS_AND_COMMAND_PORT 0x64

// Controller commands
#define PS2_READ_CONFIG_BYTE            0x20
#define PS2_WRITE_CONFIG_BYTE           0x60
#define PS2_DISABLE_FIRST_PORT          0xAD
#define PS2_ENABLE_FIRST_PORT           0xAE
#define PS2_DISABLE_SECOND_PORT         0xA7
#define PS2_ENABLE_SECOND_PORT          0xA8
#define PS2_TEST_CONTROLLER             0xAA
#define PS2_TEST_FIRST_PORT             0xAB
#define PS2_TEST_SECOND_PORT            0xA9
#define PS2_NEXT_BYTE_USES_SECOND_PORT  0xD4

// Tests
#define PS2_CONTROLLER_TEST_PASSED      0x55
#define PS2_CONTROLLER_TEST_FAILED      0xFC
#define PS2_PORT_TEST_PASSED            0

// Status
#define PS2_OUTPUT_BUFFER_FULL          1
#define PS2_INPUT_BUFFER_FULL           2

#define PS2_RESET_DEVICE                0xFF
#define PS2_ACK                         0xFA

// Mouse
#define PS2_MOUSE_USE_DEFAULTS          0xF6
#define PS2_MOUSE_ENABLE_STREAMING      0xF4
#define PS2_MOUSE_SET_SAMPLE_RATE       0xF3

namespace PS2
{
    struct Configuration
    {
        uint8_t filler2 : 1;                // Must be zero!
        uint8_t firstPortTranslation : 1;   // 1 = enabled, 0 = disabled
        uint8_t secondPortClock : 1;        // 1 = disabled, 0 = enabled
        uint8_t firstPortClock : 1;         // (as above)
        uint8_t filler : 1;                 // Must be zero!
        uint8_t systemFlag : 1;             // 1 = System passed POST, 0 = panic!
        uint8_t secondPortInterrupt : 1;    // 1 = enabled, 0 = disabled (only if there is a second port of course)
        uint8_t firstPortInterrupt : 1;     // 1 = enabled, 0 = disabled
    } __attribute__((packed));


    static_assert(sizeof(Configuration) == sizeof(uint8_t));

    void Init();
}

#endif