use multiboot2::{BootInformation, ModuleTag};
use crate::memory;
use crate::println;
use super::elf;

pub struct Module
{
    pub page_frame: memory::paging::PageFrame,
    pub entrypoint: usize
}

pub fn address_lies_within_module(address: usize, multiboot_info: &BootInformation) -> bool
{
    let count = multiboot_info.module_tags().count();

    for i in 0..count
    {
        let module = multiboot_info.module_tags().nth(i).unwrap();

        if address >= module.start_address() as usize && address <= module.end_address() as usize {
            return true;
        }
    }

    false
}

pub fn highest_module_address(multiboot_info: &BootInformation) -> usize
{
    let count = multiboot_info.module_tags().count();
    let mut highest: usize = 0;

    for i in 0..count
    {
        let module = multiboot_info.module_tags().nth(i).unwrap();
        if module.end_address() as usize > highest {
            highest = module.end_address() as usize;
        }
    }

    highest
}

pub fn load_module(module: &ModuleTag, allocator: &mut memory::allocator::PageAllocator) -> Module
{
    println!("[Multitask] Loading module {}", module.cmdline());

    let mut page_frame = memory::create_user_page_frame(allocator);

    let entrypoint = unsafe {
        elf::load_elf_file(module.start_address() as usize, allocator, &mut page_frame)
    };

    Module {
        page_frame,
        entrypoint
    }
}
