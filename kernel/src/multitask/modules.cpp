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
uint32_t nGrubModulesSize = 0;

// It is reasonable to assume the strings
// in these GRUB modules will be no bigger than
// a combined 1MB, and that each will be no bigger
// than 256 characters, so that an array of strings
// can be formed.
char* pGrubStrings = (char*)((uint32_t)(&__kernel_end) + (1024*1024*50));
constexpr uint32_t nGrubStringsLength = 256;

// Below that in memory, a 1MB array of the moduel
// sizes.
uint32_t* pGrubSizes = (uint32_t*)((uint32_t)(&__kernel_end) + (1024*1024*49));

void MoveGrubModules(multiboot_info_t* pMultiboot)
{
    // Copy GRUB modules before they're overwritten
    if (pMultiboot->mods_count > 0)
    {
        VGA_printf("");
        VGA_printf("Loading GRUB modules...");

        multiboot_module_t* module = (multiboot_module_t*) pMultiboot->mods_addr;
        for (unsigned int i = 0; i < pMultiboot->mods_count; ++i)
        {
            uint32_t size = module->mod_end - module->mod_start;

            VGA_printf("Copying from ", false);
            VGA_printf<uint32_t, true>(module->mod_start, false);
            VGA_printf(" to ", false);
            VGA_printf<uint32_t, true>((uint32_t)pGrubModules + nGrubModulesSize);
            memcpy((void*)((uint32_t)pGrubModules + nGrubModulesSize), (void*)module->mod_start, size);
            nGrubModulesSize += size;

            // Size array
            pGrubSizes[i] = size;            

            // Copy over strings too
            strncpy(pGrubStrings, (char*)module->cmdline, nGrubStringsLength);
            pGrubStrings += nGrubStringsLength;

            module++;
        }
        VGA_printf("");
    }
}

void LoadGrubModules(multiboot_info_t* pMultiboot)
{
    // Allocate GRUB buffer
    uint32_t modulePages = nGrubModulesSize;
    uint32_t moduleRemainder = modulePages % PAGE_SIZE;
    if (moduleRemainder != 0) modulePages += PAGE_SIZE + moduleRemainder;
    for (uint32_t i = 0; i < modulePages; ++i)
        AllocatePage((uint32_t)pGrubModules + PAGE_SIZE*i, (uint32_t)pGrubModules + PAGE_SIZE*i, KERNEL_PAGE, true);

    uint32_t stringsPages = pMultiboot->mods_count * nGrubStringsLength;
    uint32_t stringsRemainder = stringsPages % PAGE_SIZE;
    if (stringsRemainder != 0) stringsPages += PAGE_SIZE + stringsRemainder;
    for (uint32_t i = 0; i < stringsPages; ++i)
        AllocatePage((uint32_t)pGrubStrings + PAGE_SIZE*i, (uint32_t)pGrubStrings + PAGE_SIZE*i, KERNEL_PAGE, true);
    pGrubStrings -= pMultiboot->mods_count * nGrubStringsLength;

    uint32_t sizePages = 1024 ;
    for (uint32_t i = 0; i < sizePages; ++i)
        AllocatePage((uint32_t)pGrubSizes + PAGE_SIZE*i, (uint32_t)pGrubSizes + PAGE_SIZE*i, KERNEL_PAGE, true);
    sizePages -= pMultiboot->mods_count;

    // Load GRUB modules as user processes
    if (pMultiboot->mods_count > 0)
    {
        multiboot_module_t* module = (multiboot_module_t*) pMultiboot->mods_addr;
        for (unsigned int i = 0; i < pMultiboot->mods_count; ++i)
        {
            // Identify name of module
            char const* moduleString = pGrubStrings;
            pGrubStrings += nGrubStringsLength;

            VGA_printf("[Success] ", false, VGA_COLOUR_LIGHT_GREEN);
            VGA_printf("Loading module ", false);
            VGA_printf((char const*)moduleString, false);

            // Round size of binary to nearest page
            uint32_t originalSize = module->mod_end - module->mod_start;
            uint32_t size = originalSize;
            uint32_t remainder = size % PAGE_SIZE;
            if (remainder != 0) size += PAGE_SIZE - remainder;
            
            // Get address
            uint32_t address = (uint32_t) pGrubModules;

            VGA_printf(" at address ", false);
            VGA_printf<uint32_t, true>(address, false);
            VGA_printf(" with size ", false);
            VGA_printf<uint32_t, true>(size);

            // Module is an elf file so parse it as such
            void* program = kmalloc(size);
            memcpy(program, (void*)address, originalSize);
            auto elf = LoadElfFile(program);
            kfree(program, size);

            // Create task
            if (!elf.error)
                CreateTask((char const*)moduleString, elf.entry, elf.size, elf.location, TaskType::USER_TASK);

            module++;
            pGrubModules = (void*)((uint32_t)pGrubModules + pGrubSizes[i]);
        }   

        VGA_printf("");
    }

    // Free grub buffer
    for (uint32_t i = 0; i < modulePages; ++i)  DeallocatePage((uint32_t)pGrubModules + PAGE_SIZE*i);
    for (uint32_t i = 0; i < stringsPages; ++i) DeallocatePage((uint32_t)pGrubStrings + PAGE_SIZE*i);
    for (uint32_t i = 0; i < sizePages; ++i)    DeallocatePage((uint32_t)pGrubSizes   + PAGE_SIZE*i);
}