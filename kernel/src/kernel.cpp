#include "interrupts/interrupts.h"
#include "memory/multiboot_info.h"
#include "multitask/scheduler.h"
#include "multitask/syscalls.h"
#include "multitask/process.h"
#include "memory/memory.h"
#include "memory/smbios.h"
#include "memory/elf.h"
#include "multiboot.h"
#include "dev/uart.h"
#include "cpu/cpu.h"
#include "fs/fs.h"
#include "klib.h"

extern "C" { void kmain(multiboot_info_t* multiboot_header, uint32_t eax); }

void kmain(multiboot_info_t* multiboot_header, uint32_t eax)
{
    uart::init();

    // Verify we're multiboot and parse it
    assert(eax == MULTIBOOT_BOOTLOADER_MAGIC);
    memory::MultibootInfo info(multiboot_header);
    assert(info.n_modules >= 1);

    // Setup interrupts
    interrupts::load();

    // Setup GDT, TSS, IDT, etc.
    cpu::init();

    // Setup memory
    memory::smbios::parse();
    memory::init(info);

    // Setup filesystem, installing keyboard device
    fs::init(info, [](fs::DeviceFileSystem& dfs)
    {
        const auto on_read = [](void* data, uint64_t, uint64_t)
        {
            // Assume length to be 1
            if (interrupts::keyboard_buffer_keys == 0) return Optional<uint64_t> { 0 };
            memcpy(data, &interrupts::keyboard_buffer[interrupts::keyboard_buffer_keys-1], 1);
            interrupts::keyboard_buffer_keys--;
            return Optional<uint64_t> { 1 };
        };
        const auto on_write = [](void*, uint64_t, uint64_t) { return Optional<uint64_t>{}; };
        dfs.install(fs::DeviceFile(on_read, on_write, "keyboard"));
    });

    // Registers syscalls, etc.
    multitask::init_syscalls();
    multitask::init_scheduler(memory::kernel_frame.get_cr3());

    // Load program
    using namespace memory;
    auto user_frame = PageFrame(
        *allocate_for_kernel(PageFrame::size()),
        info.framebuffer_address,
        info.framebuffer_size,
        false
    );
    auto entry_point = *load_elf_file(user_frame, info.modules[0].address);
    auto process = multitask::Process(user_frame, entry_point);

    // Add to scheduler
    println("Loading ", info.modules[0].name);
    multitask::add_process(process);
    cpu::enable_interrupts();

    halt();
}
