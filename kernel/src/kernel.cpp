#include "interrupts/interrupts.h"
#include "memory/allocator.h"
#include "memory/multiboot_info.h"
#include "memory/elf.h"
#include "multiboot.h"
#include "io/uart.h"
#include "cpu/cpu.h"
#include "klib.h"

extern "C" { void kmain(multiboot_info_t* multiboot_header, uint32_t eax); }

extern size_t kernel_end;

void kmain(multiboot_info_t* multiboot_header, uint32_t eax)
{
    uart::init();

    // Verify we're multiboot and parse it
    assert(eax == MULTIBOOT_BOOTLOADER_MAGIC);
    memory::MultibootInfo info(multiboot_header);

    // Setup interrupts
    interrupts::load();

    // Setup GDT, TSS, IDT, etc.
    cpu::init();
    cpu::enable_interrupts();

    // Figure out where to put memory - preferably well after the kernel
    // and all the multiboot stuff
    size_t memory_start = MAX(info.memory_begin, (size_t) &kernel_end);
    memory_start = MAX(memory_start, info.get_highest_module_address());
    memory_start = memory::PageFrame::round_to_next_page_size(memory_start);

    // Setup paging, heap, etc.
    memory::Allocator root_allocator(memory_start, info.memory_end - memory_start);
    cpu::set_cr3(root_allocator.get_cr3());
    cpu::enable_paging();

    // Load program
    memory::load_elf_file(root_allocator, info.modules[0].address);

    while(1) {}
}