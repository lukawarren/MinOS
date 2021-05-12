#include "memory/modules.h"
#include "memory/memory.h"
#include "stdout/uart.h"
#include "stdlib.h"

namespace Modules
{
    /*
        Right, well, here's the dilemma... where do we put it all?
        Safest best is just to plonk it neatly just  before our
        memory runs out, so that's what I do.
    */
    uint32_t pGrubModules;
    uint32_t modulesSize;

    void Init(const multiboot_info_t* pMultiboot)
    {
        UART::WriteString("[Modules] GRUB modules: ");
        UART::WriteNumber(pMultiboot->mods_count);
        UART::WriteString("\n");

        if (pMultiboot->mods_count == 0) return;

        // Move modules to safer, known place
        multiboot_module_t* pModule = (multiboot_module_t*) pMultiboot->mods_addr;
        modulesSize = Memory::RoundToNextPageSize(pModule->mod_end - pModule->mod_start);

        // Align address to a page (our multiboot headers stipulate this too but it's nice to have a santiy check)
        pGrubModules = (uint32_t)((Memory::GetMaxMemory(pMultiboot) - modulesSize) / PAGE_SIZE) * PAGE_SIZE;
        assert(pGrubModules % PAGE_SIZE  == 0);
        memcpy((void*)pGrubModules, pModule, modulesSize);
    }

    void PostInit()
    {
        // Set modules as used in memory
        const uint32_t nPages = modulesSize / PAGE_SIZE;
        for (uint32_t i = 0; i < nPages; ++i)
        {
            const uint32_t address = pGrubModules + i*PAGE_SIZE;
            assert(Memory::IsPageSet(address) == false);
            Memory::SetPage(address, address, KERNEL_PAGE);
        }
    }
}