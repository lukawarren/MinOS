use core::mem;
use super::gdt;
use super::tss;

// GDT
pub type GdtEntry = u64;
extern "C" { fn load_gdt(base: u32, limit: u16); }

// TSS
pub static mut TSS: tss::TSS = tss::TSS::default();
extern { static __tss_stack: u32; }

pub fn init_cpu()
{
    unsafe
    {
        // Setup TSS with stack and data segment for interrupt handlers
        TSS.ss0 = 0x10; // See below - ring 0 data
        TSS.esp0 = (&__tss_stack as *const _) as u32;
        let tss_address = (&TSS as *const _) as u32;

        // Setup GDT
        let gdt: [GdtEntry; 4] =
        [
            gdt_entry(0, 0, gdt::Flag::none()),
            gdt_entry(0, 0xfffff, gdt::Flag::code_ring0()), // 0x8
            gdt_entry(0, 0xfffff, gdt::Flag::data_ring0()), // 0x10,
            gdt_entry(tss_address, mem::size_of::<tss::TSS>() as u32, gdt::Flag::tss_ring0())
        ];

        load_gdt(gdt.as_ptr() as u32, (mem::size_of::<u64>() * gdt.len()) as u16);
    }
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