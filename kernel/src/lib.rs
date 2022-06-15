#![no_std]
#![no_main]
#![feature(const_mut_refs)]

mod cpu;
mod graphics;
use core::panic::PanicInfo;

#[no_mangle]
pub extern "C" fn main() -> !
{
    unsafe { graphics::vga::GLOBAL_TEXT_DEVICE.clear() }

    println!("Initialising CPU...");
    cpu::cpu::init_cpu();

    loop {}
}

#[panic_handler]
fn panic(info: &PanicInfo) -> !
{
    println!("Error: {}", info);
    loop {}
}