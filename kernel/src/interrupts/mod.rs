mod pic;

use crate::spinlock::Lock;
use crate::arch::idt::Attributes;
use crate::arch::GLOBAL_IDT;
use crate::arch::load_idt;
use crate::arch::idt;

pub static IRQ_KEYBOARD: u32 = 1;

pub static INTERRUPT_HANDLERS: Lock<[Option<fn()>; 16]> = Lock::new([
    Option::None, Option::None, Option::None, Option::None,
    Option::None, Option::None, Option::None, Option::None,
    Option::None, Option::None, Option::None, Option::None,
    Option::None, Option::None, Option::None, Option::None
]);

pub fn init()
{
    unsafe
        {
            // Create empty IDT first
            for i in 0..GLOBAL_IDT.entries.len() {
                GLOBAL_IDT.entries[i] = idt::Entry::new(blank_irq, 0x8, Attributes::DISABLED_RING_0_INTERRUPT);
            }

            // Normal interrupts (the PIC has an offset that must be accounted for below)
            let offset = 0x20;
            GLOBAL_IDT.entries[offset + 0]  = idt::Entry::new(irq_0,  0x8, Attributes::ENABLED_RING_0_INTERRUPT);
            GLOBAL_IDT.entries[offset + 1]  = idt::Entry::new(irq_1,  0x8, Attributes::ENABLED_RING_0_INTERRUPT);
            GLOBAL_IDT.entries[offset + 2]  = idt::Entry::new(irq_2,  0x8, Attributes::ENABLED_RING_0_INTERRUPT);
            GLOBAL_IDT.entries[offset + 3]  = idt::Entry::new(irq_3,  0x8, Attributes::ENABLED_RING_0_INTERRUPT);
            GLOBAL_IDT.entries[offset + 4]  = idt::Entry::new(irq_4,  0x8, Attributes::ENABLED_RING_0_INTERRUPT);
            GLOBAL_IDT.entries[offset + 5]  = idt::Entry::new(irq_5,  0x8, Attributes::ENABLED_RING_0_INTERRUPT);
            GLOBAL_IDT.entries[offset + 6]  = idt::Entry::new(irq_6,  0x8, Attributes::ENABLED_RING_0_INTERRUPT);
            GLOBAL_IDT.entries[offset + 7]  = idt::Entry::new(irq_7,  0x8, Attributes::ENABLED_RING_0_INTERRUPT);
            GLOBAL_IDT.entries[offset + 8]  = idt::Entry::new(irq_8,  0x8, Attributes::ENABLED_RING_0_INTERRUPT);
            GLOBAL_IDT.entries[offset + 9]  = idt::Entry::new(irq_9,  0x8, Attributes::ENABLED_RING_0_INTERRUPT);
            GLOBAL_IDT.entries[offset + 10] = idt::Entry::new(irq_10, 0x8, Attributes::ENABLED_RING_0_INTERRUPT);
            GLOBAL_IDT.entries[offset + 11] = idt::Entry::new(irq_11, 0x8, Attributes::ENABLED_RING_0_INTERRUPT);
            GLOBAL_IDT.entries[offset + 12] = idt::Entry::new(irq_12, 0x8, Attributes::ENABLED_RING_0_INTERRUPT);
            GLOBAL_IDT.entries[offset + 13] = idt::Entry::new(irq_13, 0x8, Attributes::ENABLED_RING_0_INTERRUPT);
            GLOBAL_IDT.entries[offset + 14] = idt::Entry::new(irq_14, 0x8, Attributes::ENABLED_RING_0_INTERRUPT);
            GLOBAL_IDT.entries[offset + 15] = idt::Entry::new(irq_15, 0x8, Attributes::ENABLED_RING_0_INTERRUPT);

            // Exceptions
            GLOBAL_IDT.entries[0]  = idt::Entry::new(exception_0,  0x8, Attributes::ENABLED_RING_0_INTERRUPT);
            GLOBAL_IDT.entries[1]  = idt::Entry::new(exception_1,  0x8, Attributes::ENABLED_RING_0_INTERRUPT);
            GLOBAL_IDT.entries[2]  = idt::Entry::new(exception_2,  0x8, Attributes::ENABLED_RING_0_INTERRUPT);
            GLOBAL_IDT.entries[3]  = idt::Entry::new(exception_3,  0x8, Attributes::ENABLED_RING_0_INTERRUPT);
            GLOBAL_IDT.entries[4]  = idt::Entry::new(exception_4,  0x8, Attributes::ENABLED_RING_0_INTERRUPT);
            GLOBAL_IDT.entries[5]  = idt::Entry::new(exception_5,  0x8, Attributes::ENABLED_RING_0_INTERRUPT);
            GLOBAL_IDT.entries[6]  = idt::Entry::new(exception_6,  0x8, Attributes::ENABLED_RING_0_INTERRUPT);
            GLOBAL_IDT.entries[7]  = idt::Entry::new(exception_7,  0x8, Attributes::ENABLED_RING_0_INTERRUPT);
            GLOBAL_IDT.entries[8]  = idt::Entry::new(exception_8,  0x8, Attributes::ENABLED_RING_0_INTERRUPT);
            GLOBAL_IDT.entries[9]  = idt::Entry::new(exception_9,  0x8, Attributes::ENABLED_RING_0_INTERRUPT);
            GLOBAL_IDT.entries[10] = idt::Entry::new(exception_10, 0x8, Attributes::ENABLED_RING_0_INTERRUPT);
            GLOBAL_IDT.entries[11] = idt::Entry::new(exception_11, 0x8, Attributes::ENABLED_RING_0_INTERRUPT);
            GLOBAL_IDT.entries[12] = idt::Entry::new(exception_12, 0x8, Attributes::ENABLED_RING_0_INTERRUPT);
            GLOBAL_IDT.entries[13] = idt::Entry::new(exception_13, 0x8, Attributes::ENABLED_RING_0_INTERRUPT);
            GLOBAL_IDT.entries[14] = idt::Entry::new(exception_14, 0x8, Attributes::ENABLED_RING_0_INTERRUPT);
            GLOBAL_IDT.entries[15] = idt::Entry::new(exception_15, 0x8, Attributes::ENABLED_RING_0_INTERRUPT);
            GLOBAL_IDT.entries[16] = idt::Entry::new(exception_16, 0x8, Attributes::ENABLED_RING_0_INTERRUPT);
            GLOBAL_IDT.entries[17] = idt::Entry::new(exception_17, 0x8, Attributes::ENABLED_RING_0_INTERRUPT);
            GLOBAL_IDT.entries[18] = idt::Entry::new(exception_18, 0x8, Attributes::ENABLED_RING_0_INTERRUPT);
            GLOBAL_IDT.entries[19] = idt::Entry::new(exception_19, 0x8, Attributes::ENABLED_RING_0_INTERRUPT);
            GLOBAL_IDT.entries[20] = idt::Entry::new(exception_20, 0x8, Attributes::ENABLED_RING_0_INTERRUPT);
            GLOBAL_IDT.entries[21] = idt::Entry::new(exception_21, 0x8, Attributes::ENABLED_RING_0_INTERRUPT);
            GLOBAL_IDT.entries[22] = idt::Entry::new(exception_22, 0x8, Attributes::ENABLED_RING_0_INTERRUPT);
            GLOBAL_IDT.entries[23] = idt::Entry::new(exception_23, 0x8, Attributes::ENABLED_RING_0_INTERRUPT);
            GLOBAL_IDT.entries[24] = idt::Entry::new(exception_24, 0x8, Attributes::ENABLED_RING_0_INTERRUPT);
            GLOBAL_IDT.entries[25] = idt::Entry::new(exception_25, 0x8, Attributes::ENABLED_RING_0_INTERRUPT);
            GLOBAL_IDT.entries[26] = idt::Entry::new(exception_26, 0x8, Attributes::ENABLED_RING_0_INTERRUPT);
            GLOBAL_IDT.entries[27] = idt::Entry::new(exception_27, 0x8, Attributes::ENABLED_RING_0_INTERRUPT);
            GLOBAL_IDT.entries[28] = idt::Entry::new(exception_28, 0x8, Attributes::ENABLED_RING_0_INTERRUPT);
            GLOBAL_IDT.entries[29] = idt::Entry::new(exception_29, 0x8, Attributes::ENABLED_RING_0_INTERRUPT);
            GLOBAL_IDT.entries[30] = idt::Entry::new(exception_30, 0x8, Attributes::ENABLED_RING_0_INTERRUPT);

            // Finally load to CPU
            let len = core::mem::size_of::<idt::Table>() * 256 - 1;
            load_idt(&GLOBAL_IDT as *const _ as u32, len as u16);
        }

    // Setup PIC
    pic::init(pic::PIC_MASK_KEYBOARD, pic::PIC_MASK_NONE);
}

pub fn subscribe_to_irq(irq: u32, function: fn())
{
    let handlers = INTERRUPT_HANDLERS.lock();
    let index = irq as usize;

    assert_eq!(index >= handlers.len(), false);
    let _ = handlers[index].insert(function);

    INTERRUPT_HANDLERS.free();
}

#[no_mangle]
extern "C" fn on_interrupt(irq: u32)
{
    let handlers = INTERRUPT_HANDLERS.lock();
    let index = irq as usize;

    // Call subscribed interrupt handler (if any)
    if index < handlers.len() && handlers[index].is_some() {
        handlers[index].unwrap()();
    }

    else {
        panic!("unexpected IRQ {}", irq);
    }

    INTERRUPT_HANDLERS.free();
    pic::end_interrupt(irq as u8);
}

extern "C"
{
    fn blank_irq();

    fn irq_0();
    fn irq_1();
    fn irq_2();
    fn irq_3();
    fn irq_4();
    fn irq_5();
    fn irq_6();
    fn irq_7();
    fn irq_8();
    fn irq_9();
    fn irq_10();
    fn irq_11();
    fn irq_12();
    fn irq_13();
    fn irq_14();
    fn irq_15();

    fn exception_0();
    fn exception_1();
    fn exception_2();
    fn exception_3();
    fn exception_4();
    fn exception_5();
    fn exception_6();
    fn exception_7();
    fn exception_8();
    fn exception_9();
    fn exception_10();
    fn exception_11();
    fn exception_12();
    fn exception_13();
    fn exception_14();
    fn exception_15();
    fn exception_16();
    fn exception_17();
    fn exception_18();
    fn exception_19();
    fn exception_20();
    fn exception_21();
    fn exception_22();
    fn exception_23();
    fn exception_24();
    fn exception_25();
    fn exception_26();
    fn exception_27();
    fn exception_28();
    fn exception_29();
    fn exception_30();
}