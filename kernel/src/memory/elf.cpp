#include "memory/elf.h"
#include "memory/memory.h"

#define check(x) if (!(x)) { assert(x); return {}; }

Optional<size_t> memory::load_elf_file(PageFrame& user_frame, const size_t address)
{
    // Check for magic header
    auto* header = (Elf32Header*)address;
    check(header->e_ident[ElfIdent::EI_MAG_0] == ELF_MAG_0);
    check(header->e_ident[ElfIdent::EI_MAG_1] == ELF_MAG_1);
    check(header->e_ident[ElfIdent::EI_MAG_2] == ELF_MAG_2);
    check(header->e_ident[ElfIdent::EI_MAG_3] == ELF_MAG_3);

    // Check compatibility
    check(header->e_ident[EI_CLASS]   == ELF_CLASS_32);
    check(header->e_ident[EI_DATA]    == ELF_DATA_2_LSB);
    check(header->e_ident[EI_VERSION] == EV_CURRENT);
    check(header->e_machine           == EM_386);
    check(header->e_type              == ET_EXEC);

    // Load program headers
    for (size_t i = 0; i < header->e_phnum; ++i)
    {
        size_t header_address = address + header->e_phoff + sizeof(ElfProgramHeader) * i;
        auto* program_header = (ElfProgramHeader*)header_address;

        if (program_header->p_type == PT_LOAD)
        {
            if (program_header->p_filesz == 0 || program_header->p_memsz == 0)
                continue;

            // Sanity check (we're not a higher-half kernel!)
            check(program_header->p_vaddr >= memory::user_base_address);
            check(PageFrame::is_page_aligned(program_header->p_vaddr));
            check(program_header->p_align == PAGE_SIZE);

            size_t source = address + program_header->p_offset;
            size_t file_size = program_header->p_filesz;
            size_t memory_size = program_header->p_memsz;

            Optional<size_t> destination = memory::allocate_for_user(
                program_header->p_vaddr,
                memory_size,
                user_frame
            );

            if (!destination) return {};

            // If p_memsz exceeds p_filesz, then the remaining bits are to be zeroed
            memset((void*)*destination, 0, memory_size);
            memcpy((void*)*destination, (void*)source, file_size);
        }

        else if (program_header->p_type == PT_NOTE)
        {
            // Stuff for conformance, compatibility, etc.
            // Too bad.
        }

        else if (program_header->p_type == PT_GNU_STACK)
        {
            // "The p_flags member specifies the permissions on the segment containing the stack
            // and is used to indicate wether the stack should be executable. The absense of this
            // header indicates that the stack will be executable."
            // Too bad.
            check(program_header->p_memsz == 0 && program_header->p_filesz == 0);
        }

        else if (program_header->p_type == PT_GNU_RELRO)
        {
            // "The array element specifies the location and size of a segment which may be made
            // read-only after relocation shave been processed."
            // Too bad.
            check(program_header->p_memsz == 0 && program_header->p_filesz == 0);
        }

        else if (program_header->p_type == PT_GNU_EH_FRAME)
        {
            // Exception handling stuff - too bad!
        }

        else if (program_header->p_type == PT_INTERP)
        {
            // Used for dyanmic linking - shouldn't be there, but can ignore
            assert(false);
        }

        else
        {
            println("unknown program header type ", program_header->p_type);
            return {};
        }
    }

    // Load section headers
    for (size_t i = 0; i < header->e_shnum; ++i)
    {
        size_t header_address = address + header->e_shoff + sizeof(ElfSectionHeader) * i;
        auto* section_header = (ElfSectionHeader*)header_address;

        if (section_header->sh_size > 0 && (section_header->sh_flags & SHF_ALLOC))
        {
            switch (section_header->sh_type)
            {
                case SectionHeaderType::SHT_NOBITS:
                {
                    // BSS - allocate memory and zero it
                    check(PageFrame::is_page_aligned(section_header->sh_addr));
                    check(section_header->sh_addralign == PAGE_SIZE);
                    Optional<size_t> data = memory::allocate_for_user(
                        section_header->sh_addr,
                        section_header->sh_size,
                        user_frame
                    );
                    if (!data) return {};
                    memset((void*)*data, 0, section_header->sh_size);
                }
                break;

                // Already loaded by the program headers above (luckily)... I think.
                // Well it works for now and always has, so that's my story and I'm
                // sticking to it!
                case SectionHeaderType::SHT_PROGBITS: break;

                // Depending on the toolchain, this gets appened for things like
                // 'note.gnu.build-id'. It's easier to just ignore it for now.
                case SectionHeaderType::SHT_NOTE: break;

                default:
                    check(false);
            }
        }

        else if (section_header->sh_size > 0 && section_header->sh_flags & SHF_WRITE)
            check(false);
    }

    return header->e_entry;
}