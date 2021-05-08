#pragma once
#ifndef CMOS_H
#define CMOS_H

#include <stdint.h>
#include <stddef.h>

#define CMOS_ADDRESS    0x70
#define CMOS_DATA       0x71

#define CMOS_SECONDS    0x0
#define CMOS_MINUTES    0x2
#define CMOS_HOURS      0x4
#define CMOS_YEAR       0x9
#define CMOS_FORMAT     0xB

namespace CMOS
{
    struct Time
    {
        uint8_t minute;
        uint8_t hour;
    };

    Time GetTime();
}

#endif