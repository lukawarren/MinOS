use core::mem;
use core::arch::asm;
use super::gdt::*;
use super::tss::*;
use super::GLOBAL_GDT;
use super::GLOBAL_TSS;
use super::__tss_stack;
use super::load_gdt;
use super::load_tss;

pub const KERNEL_DATA_SEGMENT: u32 = 0x10;
pub const USER_CODE_SEGMENT: u32 = 0x18 | 3;
pub const USER_DATA_SEGMENT: u32 = 0x20 | 3;

pub fn init_cpu()
{
    unsafe
    {
        // Setup TSS with stack and data segment for interrupt handlers
        GLOBAL_TSS.ss0 = KERNEL_DATA_SEGMENT;
        GLOBAL_TSS.esp0 = (&__tss_stack as *const _) as u32;
        GLOBAL_TSS.iomap_base = mem::size_of::<TSS>() as u16;
        let tss_address = (&GLOBAL_TSS as *const _) as u32;

        // Descriptor: offset from start of GDT OR'ed with 3 to enable ring 3
        let tss_descriptor = (5 * mem::size_of::<u64>()) | 3;

        // Setup GDT
        GLOBAL_GDT[0] = create_gdt_entry(0, 0, Segment::NONE);
        GLOBAL_GDT[1] = create_gdt_entry(0, 0xfffff, Segment::CODE_RING_0); // 0x8
        GLOBAL_GDT[2] = create_gdt_entry(0, 0xfffff, Segment::DATA_RING_0); // 0x10,
        GLOBAL_GDT[3] = create_gdt_entry(0, 0xfffff, Segment::CODE_RING_3); // 0x18,
        GLOBAL_GDT[4] = create_gdt_entry(0, 0xfffff, Segment::DATA_RING_3); // 0x20,
        GLOBAL_GDT[5] = create_gdt_entry(tss_address, mem::size_of::<TSS>() as u32, Segment::TSS_RING_0);

        load_gdt(GLOBAL_GDT.as_ptr() as u32, (mem::size_of::<u64>() * GLOBAL_GDT.len()) as u16);
        load_tss(tss_descriptor as u16);
    }
}

pub fn outb(port: u16, data: u8)
{
    unsafe { asm!("out dx, al", in("dx") port, in("al") data); }
}

pub fn inb(port: u16) -> u8
{
    let data: u8;
    unsafe { asm!("in al, dx",  out("al") data, in("dx") port); }
    data
}

pub fn enable_interrupts()
{
    unsafe { asm!("sti"); }
}

pub fn load_cr3(address: usize)
{
    unsafe
    {
        asm!("mov cr3, {}", in(reg) address);
    }
}