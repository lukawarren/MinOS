#![allow(dead_code)]
use crate::cpu::cpu::outb;

pub const PIC_1: u16                    = 0x20;
pub const PIC_2: u16                    = 0xa0;
pub const PIC_1_COMMAND: u16            = PIC_1;
pub const PIC_2_COMMAND: u16            = PIC_2;
pub const PIC_1_DATA: u16               = PIC_1 + 1;
pub const PIC_2_DATA: u16               = PIC_2 + 1;

pub const PIC_INIT: u8                  = 0x11; // Makes the PIC wait for 3 extra init messages
pub const PIC_END_INTERRUPT: u8         = 0x20;

pub const ICW1_ICW4: u8                 = 0x1; // ICW4 command word: 0 = not needed, 1 = needed
pub const ICW1_SINGLE: u8               = 0x2; // 0 = cascade, 1 = single
pub const ICW1_INTERVAL_4: u8           = 0x4; // Call address interval: 0 = 8, 1 = 4

pub const ICW4_8086: u8                 = 0x1; // Microprocessor mode: 0 = MCS-80/85, 1 = 8086/8088
pub const ICW4_AUTO: u8                 = 0x2; // Auto EOI enabled
pub const ICW4_BUFFER_MODE_SLAVE: u8    = 0x8;
pub const ICW4_BUFFER_MODE_MASTER: u8   = 0xc;

pub const PIC_MASK_PIT_CODE: u8         = 0x1;
pub const PIC_MASK_KEYBOARD_CODE: u8    = 0x2;
pub const PIC_MASK_ALL: u8              = 0xff;
pub const PIC_MASK_NONE: u8             = 0;

pub const PIC_MASK_PIT_AND_KEYBOARD: u8 = PIC_MASK_ALL ^ PIC_MASK_PIT_CODE | PIC_MASK_KEYBOARD_CODE;
pub const PIC_MASK_KEYBOARD: u8         = PIC_MASK_ALL ^ PIC_MASK_KEYBOARD_CODE;

pub fn init(mask_one: u8, mask_two: u8)
{
    // Tell PICs to init
    outb(PIC_1_COMMAND, PIC_INIT | ICW1_ICW4);
    outb(PIC_2_COMMAND, PIC_INIT | ICW1_ICW4);

    // Remap IRQs to beginning of available range
    outb(PIC_1_DATA, 0x20); // Set master's offset to 0x20
    outb(PIC_2_DATA, 0x28); // Set slave's offset to 0x28

    outb(PIC_1_DATA, ICW1_INTERVAL_4); // Tell master PIC it has a slave
    outb(PIC_2_DATA, ICW1_SINGLE); // Tell slave PIC its cascade entity

    // Put both into 8086 mode
    outb(PIC_1_DATA, ICW4_8086);
    outb(PIC_2_DATA, ICW4_8086);

    // Set masks to control what interrupts fire
    outb(PIC_1_DATA, mask_one);
    outb(PIC_2_DATA, mask_two);
}

pub fn end_interrupt(irq: u8)
{
    if irq >= 8 { outb(PIC_2_COMMAND, PIC_END_INTERRUPT); }
    outb(PIC_1_COMMAND, PIC_END_INTERRUPT);
}