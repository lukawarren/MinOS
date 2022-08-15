#include "interrupts/interrupts.h"
#include "memory/allocator.h"
#include "multiboot.h"
#include "memory/elf.h"
#include "io/uart.h"
#include "cpu/cpu.h"
#include "klib.h"

extern "C" { void kmain(multiboot_info_t* multiboot_info, uint32_t eax); }

void kmain(multiboot_info_t* multiboot_info, uint32_t eax)
{
    uart::init();

    // Verify we're multiboot
    assert(eax == MULTIBOOT_BOOTLOADER_MAGIC);
    uart::write_string("Detected ");
    uart::write_number(multiboot_info->mods_count);
    uart::write_string(" multiboot modules\n");

    // Setup interrupts
    interrupts::load();

    // Setup GDT, TSS, IDT, etc.
    cpu::init();
    cpu::enable_interrupts();

    // Setup paging, heap, etc.
    memory::Allocator root_allocator(10 * 1024 * 1024, 10 * 1024 * 1024);
    cpu::set_cr3(root_allocator.get_cr3());
    cpu::enable_paging();

    while(1) {}
}