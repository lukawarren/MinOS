#include "interrupts/pit.h"
#include "cpu/cpu.h"

namespace pit
{
    uint64_t time_ms = 0;
    uint16_t frequency;
    uint16_t channel;

    void init()
    {
        cpu::outb
        (
            MODE_COMMAND_REGISTER,
            MODE_CHANNEL_0 |        // Use channel 0 (interrupt on terminal count)
            MODE_ACCESS_LOW_HIGH |  // Send 16 bit values to 8 bit IO port with the low value first, then high
            MODE_OPERATING_MODE_0 | // Interrupt when reload value reaches 0
            MODE_BINARY_16_BIT      // Operate in binary mode
        );
        set_frequency(CHANNEL_0_DATA, 1000);
    }

    void set_frequency(const uint16_t _channel, const uint16_t hertz)
    {
        // The PIT's oscillator runs at about 1.193182 Mhz
        const uint16_t reload_value = (uint16_t)(1193180 / (uint32_t)hertz);

        // As per "MODE_ACCESS_LOW_HIGH" above
        cpu::outb(_channel, reload_value & 0xff);
        cpu::outb(_channel, reload_value >> 8);

        // Save for later
        frequency = hertz;
        channel = _channel;
    }

    void reload()
    {
        // In the current configuration, the PIT will only fire once,
        // so we need to "reload" it after each interrupt
        time_ms += 1000 / frequency;
        set_frequency(channel, frequency);
    }
}
