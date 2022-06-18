#![no_std]
#![no_main]
#![feature(const_mut_refs)]

mod arch;
mod graphics;
mod spinlock;
mod interrupts;
mod memory;

use arch::cpu as cpu;
use core::panic::PanicInfo;
use multiboot2;

#[no_mangle]
pub extern "C" fn main(multiboot2_header_pointer: usize) -> !
{
    unsafe { graphics::vga::GLOBAL_TEXT_DEVICE.clear() }

    // Init CPU and interrupts
    cpu::init_cpu();
    interrupts::init();
    interrupts::subscribe_to_irq(interrupts::IRQ_KEYBOARD, on_keyboard);

    // Parse multiboot info
    let multiboot_header = unsafe { multiboot2::load(multiboot2_header_pointer) };
    assert_eq!(multiboot_header.is_err(), false);

    // Setup memory
    let mut root_frame = memory::PageFrame::create_root_frame(&multiboot_header.unwrap());

    println!("Welcome to MinOS!");
    println!("Free pages: {}", root_frame.free_pages_count);
    cpu::enable_interrupts();
    loop {}
}

pub fn on_keyboard()
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