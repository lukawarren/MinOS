#![no_std]
#![no_main]
#![feature(const_mut_refs)]

mod arch;
mod graphics;
mod spinlock;
mod interrupts;

use arch::cpu as cpu;

use core::panic::PanicInfo;

#[no_mangle]
pub extern "C" fn main() -> !
{
    unsafe { graphics::vga::GLOBAL_TEXT_DEVICE.clear() }

    println!("Initialising CPU...");
    cpu::init_cpu();

    println!("Initialising interrupts...");
    interrupts::init();
    interrupts::subscribe_to_irq(interrupts::IRQ_KEYBOARD, on_keyboard);

    println!("Enabling interrupts....");
    cpu::enable_interrupts();

    loop {}
}

fn on_keyboard()
{
    let key = cpu::inb(0x60);
    print!("{}", key);
}

#[panic_handler]
fn panic(info: &PanicInfo) -> !
{
    println!("Error: {}", info);
    loop {}
}