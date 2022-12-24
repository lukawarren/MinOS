#include "memory/multiboot_info.h"
#include "memory/page_frame.h"

extern size_t kernel_end;

namespace memory
{
    MultibootInfo::MultibootInfo(const multiboot_info_t* info)
    {
        parse_modules(info);
        parse_memory(info);
        parse_framebuffer(info);
    }

    void MultibootInfo::parse_modules(const multiboot_info_t* info)
    {
        n_modules = info->mods_count;
        auto* module = (multiboot_module_t*) info->mods_addr;

        for (size_t i = 0; i < n_modules; ++i)
        {
            // Copy over module info
            modules[i] = Module(
                module->mod_start,
                module->mod_end,
                (char*)module->cmdline
            );

            println("found module ", modules[i].name);
            module++;
        }
    }

    void MultibootInfo::parse_memory(const multiboot_info_t* info)
    {
        auto* memory_map = (multiboot_memory_map_t*)info->mmap_addr;

        while ((uint32_t)memory_map < info->mmap_addr + info->mmap_length)
        {
            // Find first contiguous area of memory that sounds reasonable
            if (memory_map->type == MULTIBOOT_MEMORY_AVAILABLE &&
                memory_map->addr < (uint32_t)&kernel_end &&
                memory_map->addr + memory_map->len > (uint32_t)&kernel_end)
            {
                memory_begin = (size_t) memory_map->addr;
                memory_end = memory_begin + (size_t) memory_map->len;
                return;
            }

            memory_map = (multiboot_memory_map_t*)((uint32_t)memory_map +
                            memory_map->size + sizeof(memory_map->size));
        }

        assert(false);
    }

    void MultibootInfo::parse_framebuffer(const multiboot_info_t* info)
    {
        framebuffer_address = (size_t) info->framebuffer_addr;
        framebuffer_size = info->framebuffer_pitch * info->framebuffer_height;
        println("loading framebuffer with resolution ", info->framebuffer_width, "x", info->framebuffer_height);
    }

    size_t MultibootInfo::get_highest_module_address() const
    {
        size_t highest = 0;
        for (size_t i = 0; i < n_modules; ++i)
        {
            const size_t end = modules[i].address + modules[i].size;
            highest = MAX(highest, end);
        }
        return highest;
    }
}