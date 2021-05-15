#include "multitask/elf.h"
#include "memory/memory.h"
#include "stdlib.h"

namespace Multitask
{
    uint32_t LoadElfProgram(uint32_t pFile, Memory::PageFrame& pageFrame)
    {
        // Get ELF header
        const Elf32Header* pHeader = (const Elf32Header*)pFile;
        
        // Check for magic number (0x74, 'E', 'L', 'F')
        assert(pHeader->e_ident[EI_MAG0] == ELFMAG0);
        assert(pHeader->e_ident[EI_MAG1] == ELFMAG1);
        assert(pHeader->e_ident[EI_MAG2] == ELFMAG2);
        assert(pHeader->e_ident[EI_MAG3] == ELFMAG3);

        // Check header values
        assert(pHeader->e_ident[EI_CLASS]   == ELFCLASS32);
        assert(pHeader->e_ident[EI_DATA]    == ELFDATA2LSB);
        assert(pHeader->e_ident[EI_VERSION] == EV_CURRENT);
        assert(pHeader->e_machine           == EM_386);
        assert(pHeader->e_type              == ET_EXEC);

        // Get program header
        const ElfProgramHeader* pProgramHeader = (const ElfProgramHeader*)(pFile + pHeader->e_phoff);

        // Load each section
        for (uint32_t i = 0; i < pHeader->e_phnum; ++i)
        {
            if (pProgramHeader[i].p_type != PT_LOAD) continue;

            // For now, map all pages as all readable, all executable
            void* data = pageFrame.AllocateMemory(pProgramHeader[i].p_memsz, USER_PAGE, pProgramHeader[i].p_vaddr);
            memcpy(data, (void*)(pFile + pProgramHeader[i].p_offset), pProgramHeader[i].p_memsz);
        }

        return pHeader->e_entry;
    }
}