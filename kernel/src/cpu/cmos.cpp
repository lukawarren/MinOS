#include "cpu/cmos.h"
#include "cpu/cpu.h"

namespace CMOS
{
    Time GetTime()
    {
        // Disable non maskable intrerupts
        constexpr uint8_t nmiBit = (0 << 7);

        auto GetCMOSDataFromRegister = [nmiBit](uint8_t cmosRegister)
        { 
            CPU::outb(CMOS_ADDRESS, cmosRegister | nmiBit); // Select register
            return CPU::inb(CMOS_DATA); // Read data
        };

        uint8_t minutes = GetCMOSDataFromRegister(CMOS_MINUTES);
        uint8_t hours = GetCMOSDataFromRegister(CMOS_HOURS);
        return { minutes, hours };
    }
}