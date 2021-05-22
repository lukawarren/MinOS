#pragma once
#ifndef MOUSE_H
#define MOUSE_H

/*

    IO PORT     Access Type     Purpose
    0x60        Read/Write      Data
    0x64        Read            Status
    0x64        Write           Command
*/

#define PS2_DATA_PORT 0x60
#define PS2_STATUS_AND_COMMAND_PORT 0x64

namespace PS2
{
    struct Configuration
    {
        uint8_t firstPortInterrupt : 1;     // 1 = enabled, 0 = disabled
        uint8_t secondPortInterrupt : 1;    // 1 = enabled, 0 = disabled (only if there is a second port of course)
        uint8_t systemFlag : 1;             // 1 = System passed POST, 0 = panic!
        uint8_t filler : 1;                 // Must be zero!
        uint8_t firstPortClock : 1;         // 1 = disabled, 0 = enabled
        uint8_t secondPortClock : 1;        // (as above)
        uint8_t firstPortTranslation : 1;   // 1 = enabled, 0 = disabled
        uint8_t filler2 : 1;                // Must be zero!
    };

    void Init();
}

#endif