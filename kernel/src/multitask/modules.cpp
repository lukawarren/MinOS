#include "modules.h"
#include "../memory/paging.h"

#include "../gfx/vga.h"
#include "multitask.h"
#include "elf.h"

// Temporary place to store GRUB modules
// Paging ruins memory, but a modern machine
// really should have at least 50MB (right?!),
// so just put it there
void* pGrubModules = (void*)((uint32_t)(&__kernel_end) + (1024*1024*60)); 
uint32_t grubModulesSize = 0;

void MoveGrubModules(multiboot_info_t* pMultiboot)
{
    // Copy GRUB modules before they're overwritten
    if (pMultiboot->mods_count > 0)
    {
        multiboot_module_t* module = (multiboot_module_t*) pMultiboot->mods_addr;
        uint32_t size = module->mod_end - module->mod_start;
        grubModulesSize = size;
        memcpy((void*)((uint32_t)pGrubModules), (void*)module->mod_start, size);      
    }
    else
    {
        VGA_printf("[Failure]", false, VGA_COLOUR_LIGHT_RED);
        VGA_printf("Failed to find a GRUB module!");
    }
}

uint32_t LoadGrubVFS(multiboot_info_t* pMultiboot)
{
    // Allocate GRUB buffer
    uint32_t moduleSizeRemainder = grubModulesSize % PAGE_SIZE;
    if (moduleSizeRemainder != 0) grubModulesSize += PAGE_SIZE + moduleSizeRemainder;
    uint32_t modulePages = grubModulesSize / PAGE_SIZE;

    for (uint32_t i = 0; i < modulePages; ++i)
        AllocatePage((uint32_t)pGrubModules + PAGE_SIZE*i, (uint32_t)pGrubModules + PAGE_SIZE*i, KERNEL_PAGE, true);

    void* pGrubModulesOriginal = pGrubModules;

    // Load GRUB modules as user processes
    uint32_t vfs = 0;
    if (pMultiboot->mods_count > 0)
    {
        multiboot_module_t* module = (multiboot_module_t*) pMultiboot->mods_addr;
        for (unsigned int i = 0; i < pMultiboot->mods_count; ++i)
        {
            // Round size of file to nearest page
            uint32_t originalSize = module->mod_end - module->mod_start;
            uint32_t size = originalSize;
            uint32_t remainder = size % PAGE_SIZE;
            if (remainder != 0) size += PAGE_SIZE - remainder;
            
            // Get address
            uint32_t address = (uint32_t) pGrubModules;

            // Allocate memory
            void* memory = kmalloc(size);
            memcpy(memory, (void*)address, originalSize);
            vfs = (uint32_t)memory;
        }
    }
    else
    {
        VGA_printf("[Failure]", false, VGA_COLOUR_LIGHT_RED);
        VGA_printf("Failed to load a GRUB module!");
    }

    pGrubModules = pGrubModulesOriginal;

    // Free grub buffer
    for (uint32_t i = 0; i < modulePages; ++i)  DeallocatePage((uint32_t)pGrubModules + PAGE_SIZE*i);

    return vfs;
}