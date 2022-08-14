#include "interrupts/interrupts.h"
#include "memory/pageFrame.h"
#include "io/uart.h"
#include "cpu/cpu.h"
#include "klib.h"

extern "C" { void kmain(void); }

void kmain(void) 
{
    uart::init();

    // Setup interrupts
    interrupts::load();

    // Setup GDT, TSS, IDT, etc.
    cpu::init();
    cpu::enable_interrupts();

    memory::PageFrame f(10485760);
    cpu::set_cr3(f.get_cr3());
    cpu::enable_paging();

    while(1) {}
}