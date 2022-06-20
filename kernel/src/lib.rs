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
    let multiboot_info = unsafe { multiboot2::load(multiboot2_header_pointer) }.unwrap();

    // Setup memory
    let (mut allocator, mut frame) = memory::init(&multiboot_info);

    // Load module
    let _module = multitask::module::load_module(multiboot_info.module_tags().nth(0).unwrap(), &mut allocator);

    unsafe
    {
        // Allocate page for test
        let addr = allocator.allocate_kernel_page();
        println!("First free page {:#x}", addr);
        allocator.free_kernel_page(addr, &mut frame);
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