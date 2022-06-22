#![allow(dead_code)]
use crate::arch::cpu::outb;
use crate::spinlock::Lock;

const CHANNEL_0_DATA: u16         = 0x40;
const CHANNEL_1_DATA: u16         = 0x41;
const CHANNEL_2_DATA: u16         = 0x42;
const MODE_COMMAND_REGISTER: u16  = 0x43;

const MODE_CHANNEL_0: u8          = 0b00000000;
const MODE_CHANNEL_1: u8          = 0b01000000;
const MODE_CHANNEL_2: u8          = 0b10000000;

const MODE_ACCESS_LATCH: u8       = 0b00000000;
const MODE_ACCESS_LOW_BYTE: u8    = 0b00010000;
const MODE_ACCESS_HIGH_BYTE: u8   = 0b00100000;
const MODE_ACCESS_LOW_HIGH: u8    = 0b00110000;

const MODE_OPERATING_MODE_0: u8   = 0b00000000;
const MODE_OPERATING_MODE_1: u8   = 0b00000010;
const MODE_OPERATING_MODE_2: u8   = 0b00000100;
const MODE_OPERATING_MODE_3: u8   = 0b00000110;
const MODE_OPERATING_MODE_4: u8   = 0b00001000;
const MODE_OPERATING_MODE_5: u8   = 0b00001010;

const MODE_BINARY_16_BIT: u8      = 0;
const MODE_BINARY_FOUR_DIGIT: u8  = 1;

static FREQUENCY: Lock<u16> = Lock::new(0);
static CHANNEL: Lock<u16> = Lock::new(0);

pub fn init()
{
    // Set operating mode
    outb(MODE_COMMAND_REGISTER, MODE_CHANNEL_0 |        // Use channel 0 (interrupt on terminal count)
                                MODE_ACCESS_LOW_HIGH |  // Send 16 bit values to 8 bit IO port with the low value first, then high
                                MODE_OPERATING_MODE_0 | // Interrupt when reload value reaches 0
                                MODE_BINARY_16_BIT);    // Operate in binary mode

    set_frequency(CHANNEL_0_DATA, 60, true);
}

/// Needs to be called after each PIT interrupt, because in the set mode they only fire once
pub fn reload()
{
    set_frequency(*CHANNEL.lock(), *FREQUENCY.lock(), false);
    CHANNEL.free();
    FREQUENCY.free();
}

fn set_frequency(channel: u16, hertz: u16, save_config: bool)
{
    // The PIT's oscillator runs at about 1.193182 Mhz
    let reload_value = (1193180 / hertz as u32) as u16;

    // As per "MODE_ACCESS_LOW_HIGH" above, set frequency
    outb(channel, (reload_value & 0xff) as u8);
    outb(channel, (reload_value >> 8) as u8);

    // Store for later
    if save_config
    {
        *FREQUENCY.lock() = hertz;
        *CHANNEL.lock() = channel;
        FREQUENCY.free();
        CHANNEL.free();
    }
}
