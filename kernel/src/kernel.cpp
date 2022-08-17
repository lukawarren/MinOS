#include "interrupts/interrupts.h"
#include "memory/multiboot_info.h"
#include "memory/memory.h"
#include "memory/elf.h"
#include "multiboot.h"
#include "io/uart.h"
#include "cpu/cpu.h"
#include "klib.h"

extern "C" { void kmain(multiboot_info_t* multiboot_header, uint32_t eax); }


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

    // Setup memory
    memory::init(info);

    // Load program
    using namespace memory;
    auto user_frame = PageFrame((size_t) allocate_for_kernel(PageFrame::size()), true);
    load_elf_file(user_frame, info.modules[0].address);
    cpu::set_cr3(user_frame.get_cr3());

    while(1) {}
}