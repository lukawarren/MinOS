use core::mem;
use super::gdt;
use crate::println;

pub type GdtEntry = u64;

extern "C" {
    fn load_gdt(base: u32, limit: u16);
}

pub fn init_cpu()
{
    // Setup GDT
    let gdt: [GdtEntry; 3] =
    [
        gdt_entry(0, 0, gdt::Flag::none()),
        gdt_entry(0, 0xfffff, gdt::Flag::code_ring0()),
        gdt_entry(0, 0xfffff, gdt::Flag::data_ring0())
    ];

    unsafe { load_gdt(gdt.as_ptr() as u32, (mem::size_of::<u64>() * gdt.len()) as u16); }
    println!("Loaded GDT values {:#?}", gdt);
}

fn gdt_entry(base: u32, limit: u32, flags: gdt::Segment) -> GdtEntry
{
    let mut descriptor: u64 = 0;
    let flag_bits = flags.bits() as u64;

    // Create the high 32 bit segment
    descriptor |= limit as u64          & 0x000f0000; // Bits 19:16
    descriptor |= (flag_bits << 8)      & 0x00f0ff00; // Set type, p, dpl, s, g, d/b, l and avl fields
    descriptor |= ((base as u64) >> 16) & 0x000000ff; // Base bits 23:16
    descriptor |= base as u64           & 0xff000000; // Base bits 31:24

    // Fill out the low segment
    descriptor <<= 32;
    descriptor |= (base as u64) << 16;
    descriptor |= limit as u64 & 0x0000ffff;

    descriptor
}