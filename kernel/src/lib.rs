#![no_std]
#![no_main]
#![feature(const_mut_refs)]

mod graphics;
use core::panic::PanicInfo;

#[no_mangle]
pub extern "C" fn main() -> !
{
    unsafe { graphics::vga::GLOBAL_TEXT_DEVICE.clear() }
    println!("Hello world from {}", "Rust!");
    loop {}
}

#[panic_handler]
fn panic(info: &PanicInfo) -> !
{
    println!("Error: {}", info);
    loop {}
}