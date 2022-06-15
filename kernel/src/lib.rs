#![no_std]
#![no_main]
#![feature(const_mut_refs)]

mod cpu;
mod graphics;
mod spinlock;
use core::panic::PanicInfo;

#[no_mangle]
pub extern "C" fn main() -> !
{
    unsafe { graphics::vga::GLOBAL_TEXT_DEVICE.clear() }

    println!("Initialising CPU...");
    cpu::cpu::init_cpu();

    use spinlock::Lock;
    let test = Lock::<bool>::new(false);

    println!("Locking...");
    let mut bob = test.lock();
    *bob = true;
    println!("{}", bob);
    println!("Freed...");
    test.free();


    println!("Locking once... {}", test.lock());
    println!("Locking twice (should fail) {}", test.lock());

    loop {}
}

#[panic_handler]
fn panic(info: &PanicInfo) -> !
{
    println!("Error: {}", info);
    loop {}
}