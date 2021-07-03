#pragma once
#ifndef CMOS_H
#define CMOS_H

#include <stdint.h>
#include <stddef.h>

#define CMOS_ADDRESS            0x70
#define CMOS_DATA               0x71

#define CMOS_SECONDS            0x0
#define CMOS_MINUTES            0x2
#define CMOS_HOURS              0x4
#define CMOS_DAY                0x7
#define CMOS_MONTH              0x8
#define CMOS_YEAR               0x9
#define CMOS_FORMAT             0xB
#define CMOS_STATUS_A           0xA
#define CMOS_STATUS_B           0xB

#define CMOS_STATUS_B_BINARY    0x4
#define CMOS_STATUS_B_24_HR     0x2

namespace CMOS
{
    struct Time
    {
        uint32_t second;
        uint32_t minute;
        uint32_t hour;
        uint32_t day;
        uint32_t month;
        uint32_t year;

        uint32_t GetSecondsInDay()
        {
            return
                hour * 60 * 60 +
                minute * 60 +
                second;
        }
    };

    Time GetTime();
}

#endif