#include "memory/modules.h"
#include "memory/memory.h"
#include "stdout/uart.h"
#include "stdlib.h"

namespace Modules
{
    /*
        Code is nice and simple because we only have the one module
    */

    uint32_t pGrubModule;
    uint32_t moduleSize;

    void Init(const multiboot_info_t* pMultiboot)
    {
        UART::WriteString("[Modules] GRUB modules: ");
        UART::WriteNumber(pMultiboot->mods_count);
        UART::WriteString("\n");

        assert(pMultiboot->mods_count == 1);

        // Move module to a safer, known place
        multiboot_module_t* pModule = (multiboot_module_t*) pMultiboot->mods_addr;
        moduleSize = Memory::RoundToNextPageSize(pModule->mod_end - pModule->mod_start);

        // Pick end of memory then align address to a page
        pGrubModule = Memory::GetMaxMemory(pMultiboot) - moduleSize;
        pGrubModule /= PAGE_SIZE;
        pGrubModule *= PAGE_SIZE;
        memcpy((void*)pGrubModule, (void*)pModule->mod_start, moduleSize);
    }

    void PostInit()
    {
        // Set module as used in memory
        const uint32_t nModulePages = moduleSize / PAGE_SIZE;
        for (uint32_t i = 0; i < nModulePages; ++i)
        {
            const uint32_t address = pGrubModule + i*PAGE_SIZE;
            assert(Memory::kPageFrame.IsPageSet(address) == false);
            Memory::kPageFrame.SetPage(address, address, KERNEL_PAGE);
        }
    }

    uint32_t GetModule()
    {
        return (uint32_t)pGrubModule;
    }
}