#![no_std]
#![no_main]
#![feature(const_mut_refs)]

mod arch;
mod graphics;
mod spinlock;
mod interrupts;
mod memory;
mod multitask;

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

    println!("{:#?}", multiboot_header.as_ref().unwrap().module_tags());

    // Setup memory
    let (mut allocator, frame) = memory::init(multiboot_header.as_ref().unwrap());

    unsafe
    {
        // Allocate page for test
        let addr = allocator.allocate_kernel_page();
        println!("First free page {:#x}", addr);
        allocator.free_kernel_page(addr, &frame);
    }

    println!("Free pages: {}", allocator.free_pages_count);
    println!("Welcome to MinOS!");
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