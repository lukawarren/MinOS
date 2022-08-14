#include "interrupts/interrupts.h"
#include "memory/allocator.h"
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

    // Setup paging, heap, etc.
    memory::Allocator rootAllocator(10 * 1024 * 1024, 10 * 1024 * 1024);
    cpu::set_cr3(rootAllocator.get_cr3());
    cpu::enable_paging();

    while(1) {}
}