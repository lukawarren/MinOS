#include "memory/modules.h"
#include "memory/memory.h"
#include "stdout/uart.h"
#include "stdlib.h"

namespace Modules
{
    /*
        Right, well, here's the dilemma... where do we put it all?
        Safest best is just to plonk it neatly just before our
        memory runs out, so that's what I do.
    */
    uint32_t pGrubModules;
    uint32_t modulesSize;
    uint32_t pGrubCommandlines;
    uint32_t commandlinesSize;

    void Init(const multiboot_info_t* pMultiboot)
    {
        UART::WriteString("[Modules] GRUB modules: ");
        UART::WriteNumber(pMultiboot->mods_count);
        UART::WriteString("\n");

        if (pMultiboot->mods_count == 0) return;

        // Move modules to safer, known place
        multiboot_module_t* pModule = (multiboot_module_t*) pMultiboot->mods_addr;
        modulesSize = Memory::RoundToNextPageSize(pModule->mod_end - pModule->mod_start);

        // Pick end of memory then align address to a page
        pGrubModules = Memory::GetMaxMemory(pMultiboot) - modulesSize;
        pGrubModules /= PAGE_SIZE;
        pGrubModules *= PAGE_SIZE;
        memcpy((void*)pGrubModules, (void*)pModule->mod_start, modulesSize);

        // Get total commandlines size
        commandlinesSize = 0;
        for (uint32_t i = 0; i < pMultiboot->mods_count; ++i)
        {
            commandlinesSize += strlen((char*) pModule->cmdline);
            pModule++;
        }

        // Pick address
        pGrubCommandlines = pGrubModules - commandlinesSize;
        pGrubCommandlines /= PAGE_SIZE; // Align to nearest page
        pGrubCommandlines *= PAGE_SIZE; // Align to nearest page

        // Move each string
        uint32_t copyAddress = pGrubCommandlines;
        pModule = (multiboot_module_t*) pMultiboot->mods_addr;
        for (uint32_t i = 0; i < pMultiboot->mods_count; ++i)
        {
            strcpy((char*)copyAddress, (char*)pModule->cmdline);
            copyAddress += strlen((char*)pModule->cmdline);
            strcpy((char*)copyAddress, "");
            copyAddress++;
            pModule++;
        }
    }

    void PostInit(const multiboot_info_t* pMultiboot)
    {
        if (pMultiboot->mods_count == 0) return;

        // Set modules as used in memory
        const uint32_t nModulePages = modulesSize / PAGE_SIZE;
        for (uint32_t i = 0; i < nModulePages; ++i)
        {
            const uint32_t address = pGrubModules + i*PAGE_SIZE;
            assert(Memory::IsPageSet(address) == false);
            Memory::SetPage(address, address, KERNEL_PAGE);
        }

        // Do the same for commandlines
        const uint32_t nCommandlinePages = Memory::RoundToNextPageSize(commandlinesSize) / PAGE_SIZE;
        for (uint32_t i = 0; i < nCommandlinePages; ++i)
        {
            const uint32_t address = pGrubCommandlines + i*PAGE_SIZE;
            assert(Memory::IsPageSet(address) == false);
            Memory::SetPage(address, address, KERNEL_PAGE);
        }

        // Enumerate modules
        multiboot_module_t* pModule = (multiboot_module_t*) pGrubModules;
        for (uint32_t i = 0; i < pMultiboot->mods_count; ++i)
        {
            UART::WriteString("[Modules] Found ");
            UART::WriteString((const char*)pGrubCommandlines);
            UART::WriteString("\n");

            // Move commandilnes to next null terminator
            pGrubCommandlines += strlen((char*) pGrubCommandlines) + 1;

            pModule++;
        }

    }
}