#include "memory/elf.h"

size_t memory::load_elf_file(memory::Allocator& allocator, const size_t address)
{
    // Check for magic header
    auto* header = (Elf32Header*)address;
    assert(header->e_ident[ElfIdent::EI_MAG_0] == ELF_MAG_0);
    assert(header->e_ident[ElfIdent::EI_MAG_1] == ELF_MAG_1);
    assert(header->e_ident[ElfIdent::EI_MAG_2] == ELF_MAG_2);
    assert(header->e_ident[ElfIdent::EI_MAG_3] == ELF_MAG_3);

    // Check compatibility
    assert(header->e_ident[EI_CLASS] == ELF_CLASS_32);
    assert(header->e_ident[EI_DATA] == ELF_DATA_2_LSB);
    assert(header->e_ident[EI_VERSION] == EV_CURRENT);
    assert(header->e_machine == EM_386);
    assert(header->e_type == ET_EXEC);

    // Load program headers
    for (size_t i = 0; i < header->e_phnum; ++i)
    {
        size_t header_address = address + header->e_phoff + sizeof(ElfProgramHeader) * i;
        auto* program_header = (ElfProgramHeader*)header_address;

        if (program_header->p_type == PT_LOAD)
        {
            // Sanity check (we're not a higher-half kernel!)
            assert(program_header->p_vaddr >= 0x40000000);
            assert(PageFrame::is_page_aligned(program_header->p_vaddr));

            size_t source = address + program_header->p_offset;
            size_t file_size = program_header->p_filesz;
            size_t memory_size = program_header->p_memsz;

            size_t destination = (size_t) allocator.get_user_memory_at_address(
                program_header->p_vaddr, memory_size
            );

            // If p_memsz exceeds p_filesz, then the remaining bits are to be zeroed
            memset((void*)destination, 0, memory_size);
            memcpy((void*)destination, (void*)source, file_size);
        }
    }

    // Load section headers
    for (size_t i = 0; i < header->e_shnum; ++i)
    {
        size_t header_adderess = address + header->e_shoff + sizeof(ElfSectionHeader) * i;
        auto* section_header = (ElfSectionHeader*)header_adderess;

        if (section_header->sh_size > 0 && (section_header->sh_flags & SHF_ALLOC))
        {
            switch (section_header->sh_type)
            {
                case SectionHeaderType::SHT_PROGBITS:
                    // Already loaded by the program headers above (luckily)... I think.
                    // Well it works for now and always has, so that's my story and I'm
                    // sticking to it!
                    break;

                default:
                    assert(false);
            }
        }

        else if (section_header->sh_flags & SHF_WRITE) assert(false);
    }

    return header->e_entry;
}