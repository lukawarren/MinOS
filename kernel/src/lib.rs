#![no_std]
#![no_main]
#![feature(const_mut_refs)]

mod arch;
mod graphics;
mod spinlock;
mod interrupts;
mod memory;
mod multitask;
mod syscalls;

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

    // Parse multiboot info and setup memory
    let multiboot_info = unsafe { multiboot2::load(multiboot2_header_pointer) }.unwrap();
    let (mut allocator, page_frame) = memory::init(&multiboot_info);

    // Load tasks
    for module in multiboot_info.module_tags()
    {
        println!("[Multitask] Loading module {}", module.cmdline());
        let task = multitask::module::load_module(module, &mut allocator);
        multitask::add_task(task);
    }

    // Hand over allocator, etc to future syscalls
    syscalls::init(allocator, page_frame);

    println!("Welcome to MinOS!");
    cpu::enable_interrupts();
    loop {}
}

pub fn on_keyboard(_: &cpu::Registers)
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