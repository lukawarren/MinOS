#include "interrupts/interrupts.h"
#include "memory/multiboot_info.h"
#include "multitask/scheduler.h"
#include "multitask/syscalls.h"
#include "multitask/process.h"
#include "memory/memory.h"
#include "memory/smbios.h"
#include "memory/elf.h"
#include "multiboot.h"
#include "dev/keyboard.h"
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
    const memory::MultibootInfo info(multiboot_header);
    assert(info.n_modules >= 1);

    // Setup interrupts
    interrupts::load();

    // Setup GDT, TSS, IDT, etc.
    cpu::init();

    // Setup memory
    memory::smbios::parse();
    memory::init(info);

    // Setup filesystem, installing devices
    fs::init(info, [](fs::DeviceFileSystem& dfs)
    {
        // Keyboard - disregard offset
        {
            const auto on_read = [](void* data, uint64_t, uint64_t length)
            {
                uint64_t len = MIN(keyboard::n_scancodes, length);
                memcpy_large(data, &keyboard::scancodes, len);
                return Optional<uint64_t> { len };
            };
            const auto on_write = [](void*, uint64_t, uint64_t) { return Optional<uint64_t>{}; };
            dfs.install(fs::DeviceFile(on_read, on_write, "keyboard"));
        }

        // UART - assume length to be 1 and offset to be 0
        {
            const auto on_read = [](void* data, uint64_t, uint64_t)
            {
                const Optional<char> input = uart::read_char();
                if (!input) return Optional<uint64_t> { 0 };
                *(char*)data = input.data;
                return Optional<uint64_t> { 1 };
            };
            const auto on_write = [](void*, uint64_t, uint64_t) { return Optional<uint64_t>{}; };
            dfs.install(fs::DeviceFile(on_read, on_write, "uart"));
        }
    });

    // Registers syscalls, etc.
    multitask::init_syscalls();
    multitask::init_scheduler(memory::kernel_frame.get_cr3());

    // Jump to userspace
    memory::add_elf_from_module(info, "minwm.bin");
    memory::add_elf_from_module(info, "minshell.bin");
    memory::add_elf_from_module(info, "doom.bin");
    memory::add_elf_from_module(info, "snake.bin");
    println("Entering userspace...");
    cpu::enable_interrupts();

    halt();
}
