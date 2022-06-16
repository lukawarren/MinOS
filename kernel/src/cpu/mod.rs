pub mod cpu;
pub mod idt;
mod gdt;
mod tss;

use gdt::GdtEntry;
use idt::Table;
use tss::TSS;

// Need to be kept global because the CPU will refer to these for its whole lifetime
pub static mut GLOBAL_GDT: [GdtEntry; 4] = [ 0, 0, 0, 0 ];
pub static mut GLOBAL_IDT: Table = Table::default();
pub static mut GLOBAL_TSS: TSS = TSS::default();

extern "C"
{
    pub fn load_gdt(base: u32, limit: u16);
    pub fn load_idt(base: u32, limit: u16);
    pub static __tss_stack: u32;
}