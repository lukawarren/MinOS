#include "cpu/cmos.h"
#include "cpu/cpu.h"

namespace CMOS
{
    constexpr uint8_t nmiBit = (0 << 7); // Disable non maskable intrerupts

    static uint8_t ReadRegister(const uint8_t reg)
    {
        CPU::outb(CMOS_ADDRESS, reg | nmiBit); // Select register
        return CPU::inb(CMOS_DATA); // Read data
    }

    static bool IsBusy()
    {
        return ReadRegister(CMOS_STATUS_A) & 0x80;
    }

    static uint32_t BCDToBinary(const uint32_t bcd)
    {
        return (bcd & 0xf) + ((bcd >> 4) * 10);
    }

    Time GetTime()
    {
        while (IsBusy()) {}   

        // Get time
        uint32_t seconds = ReadRegister(CMOS_SECONDS);
        uint32_t minutes = ReadRegister(CMOS_MINUTES);
        uint32_t hours   = ReadRegister(CMOS_HOURS);
        uint32_t day     = ReadRegister(CMOS_DAY);
        uint32_t month   = ReadRegister(CMOS_MONTH);
        uint32_t year    = ReadRegister(CMOS_HOURS);

        // Convert from BCD to binary
        const uint8_t statusB = ReadRegister(CMOS_STATUS_B);
        if ((statusB & CMOS_STATUS_B_BINARY) == 0)
        {
            seconds = BCDToBinary(seconds);
            minutes = BCDToBinary(minutes);
            hours   = BCDToBinary(hours & 0x7F);
            day     = BCDToBinary(day);
            month   = BCDToBinary(month);
            year    = BCDToBinary(year);
        }

        // Convert hour to 24-hour time if need be
        if ((statusB & CMOS_STATUS_B_24_HR) == false && (hours & 0x80))
        {
            hours = ((hours & 0x7f) + 12) % 24;
        }

        year += 2000; // TODO: Fix when time travel is invented
        hours += 1; // Don't ask me why it's +1! BST? Too bad!
        return { seconds, minutes, hours, day, month, year };
    }
}