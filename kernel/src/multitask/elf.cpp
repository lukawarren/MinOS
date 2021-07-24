#include "multitask/elf.h"
#include "memory/memory.h"
#include "kstdlib.h"

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

        // Get program headers
        const ElfProgramHeader* pProgramHeaders = (const ElfProgramHeader*)(pFile + pHeader->e_phoff);

        // Load each program header
        for (uint32_t i = 0; i < pHeader->e_phnum; ++i)
        {
            if (pProgramHeaders[i].p_type != PT_LOAD) continue;

            // For now, map all pages as all readable, all executable
            void* data = pageFrame.AllocateMemory(pProgramHeaders[i].p_memsz, USER_PAGE, pProgramHeaders[i].p_vaddr);
            memcpy(data, (void*)(pFile + pProgramHeaders[i].p_offset), pProgramHeaders[i].p_memsz);
        }

        // Get section headers
        const ElfSectionHeader* pSectionHeaders = (const ElfSectionHeader*)(pFile + pHeader->e_shoff);

        // Load section headers
        for (uint32_t i = 0; i < pHeader->e_shnum; ++i)
        {
            const auto type =  pSectionHeaders[i].sh_type;
            const auto size =  pSectionHeaders[i].sh_size;
            const auto flags = pSectionHeaders[i].sh_flags;

            if (size == 0) continue;
            if ((flags & SectionHeaderFlags::SHF_ALLOC) == false) continue;

            switch (type)
            {
                
                case SectionHeaderType::SHT_NOBITS: // BSS
                {
                    // Allocate memory and zero it
                    void* data = pageFrame.AllocateMemory(size, USER_PAGE, pSectionHeaders[i].sh_addr);
                    memset(data, size, 0);
                    break;
                }

                case SectionHeaderType::SHT_PROGBITS:
                { 
                    // If it's time to figure out issues with identity (or lack thereof) mapping, we're in trouble, and make it double!
                    assert(pSectionHeaders[i].sh_addr >= USER_PAGING_OFFSET);

                    //  .text, .text.startup, .rodata and .data actually get loaded (by pure luck I might add) by our program headers
                    // so all is well with the world (well the flags might be a bit off bit you can't have everything)
                    break;
                }

                case SectionHeaderType::SHT_FINI_ARRAY:
                {
                    // As above
                    assert(pSectionHeaders[i].sh_addr >= USER_PAGING_OFFSET);
                    break;
                }

                case SectionHeaderType::SHT_STRTAB:
                    assert(false);
                break;

                default:
                    assert(false);
                break;
            }
        }

        return pHeader->e_entry;
    }
}