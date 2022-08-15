#include "interrupts/interrupts.h"
#include "memory/allocator.h"
#include "memory/elf.h"
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
    memory::Allocator root_allocator(10 * 1024 * 1024, 10 * 1024 * 1024);
    cpu::set_cr3(root_allocator.get_cr3());
    cpu::enable_paging();

    auto* a = root_allocator.allocate_pages(1);
    root_allocator.free_pages((uint32_t)&a, 1);

    while(1) {}
}