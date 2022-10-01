#pragma once
#include "klib.h"
#include "memory/allocator.h"
#include "memory/page_frame.h"
#include "memory/multiboot_info.h"
#include "multitask/process.h"

// e_ident data
#define ELF_MAG_0       0x7F    // e_ident[EI_MAG0]
#define ELF_MAG_1       'E'     // e_ident[EI_MAG1]
#define ELF_MAG_2       'L'     // e_ident[EI_MAG2]
#define ELF_MAG_3       'F'     // e_ident[EI_MAG3]
#define ELF_DATA_2_LSB   1      // Little Endian
#define ELF_CLASS_32     1      // 32-bit Architecture

// Type
#define EM_386      (3)     // x86 Machine Type
#define EV_CURRENT  (1)     // ELF Current Version

namespace memory
{
    typedef uint16_t elf32_half;        // Unsigned half int
    typedef uint32_t elf32_offset;      // Unsigned offset
    typedef uint32_t elf32_address;     // Unsigned address
    typedef uint32_t elf32_word;        // Unsigned int
    typedef int32_t  elf32_signedWord;  // Signed int

    // Elf header
    struct Elf32Header
    {
        uint8_t         e_ident[16];
        elf32_half      e_type;
        elf32_half      e_machine;
        elf32_word      e_version;
        elf32_address   e_entry;
        elf32_offset    e_phoff;
        elf32_offset    e_shoff;
        elf32_word      e_flags;
        elf32_half      e_ehsize;
        elf32_half      e_phentsize;
        elf32_half      e_phnum;
        elf32_half      e_shentsize;
        elf32_half      e_shnum;
        elf32_half      e_shstrndx;
    } __attribute__((packed));

    // Elf e_ident
    enum ElfIdent
    {
        EI_MAG_0       = 0, // 0x7F
        EI_MAG_1       = 1, // 'E'
        EI_MAG_2       = 2, // 'L'
        EI_MAG_3       = 3, // 'F'
        EI_CLASS       = 4, // Architecture (32/64)
        EI_DATA        = 5, // Byte Order
        EI_VERSION     = 6, // ELF Version
        EI_OSABI       = 7, // OS Specific
        EI_ABIVERSION  = 8, // OS Specific
        EI_PAD         = 9  // Padding
    };

    enum ElfType
    {
        ET_NONE        = 0,    // Unknown
        ET_REL         = 1,    // Relocatable file
        ET_EXEC        = 2     // Executable file
    };

    // Program header
    struct ElfProgramHeader
    {
        elf32_word      p_type;
        elf32_word      p_offset;
        elf32_address   p_vaddr;
        elf32_address   p_paddr;
        elf32_word      p_filesz;
        elf32_word      p_memsz;
        elf32_address   p_flags;
        elf32_address   p_align;
    } __attribute__((packed));

    // Program header p_type
    enum ProgramHeaderType
    {
        PT_NULL = 0,    // Unused
        PT_LOAD = 1,    // Loadable segment
        PT_DYNAMIC = 2, // Dynamic linking information
        PT_INTERP = 3,  // More linking stuff
        PT_NOTE = 4,
        PT_GNU_EH_FRAME = 0x6474e550, // Exception handling information
        PT_GNU_STACK = 0x6474e551, // Controls executable permissions of stack segment
        PT_GNU_RELRO = 0x6474e552, // May be made read-only after relocation shave been processed
    };

    // Section header
    struct ElfSectionHeader
    {
        elf32_word      sh_name;
        elf32_word      sh_type;
        elf32_word      sh_flags;
        elf32_address   sh_addr;
        elf32_offset    sh_offset;
        elf32_word      sh_size;
        elf32_word      sh_link;
        elf32_word      sh_info;
        elf32_word      sh_addralign;
        elf32_word      sh_entsize;
    } __attribute__((packed));

    // Section header sh_type
    enum SectionHeaderType
    {
        SHT_NULL        = 0,  // Null section
        SHT_PROGBITS    = 1,  // Program information
        SHT_SYMTAB      = 2,  // Symbol table
        SHT_STRTAB      = 3,  // String table
        SHT_RELA        = 4,  // Relocation (with addend)
        SHT_DYNAMIC     = 6,
        SHT_NOTE        = 7,  // E.g. note.gnu.build-id
        SHT_NOBITS      = 8,  // Not present in file
        SHT_REL         = 9,  // Relocation (no addend)
        SHT_INIT_ARRAY  = 14, // Array of constructors
        SHT_FINI_ARRAY  = 15, // Array of destructors
    };

    // Section header sh_flags
    enum SectionHeaderFlags
    {
        SHF_WRITE       = 1, // Writable section
        SHF_ALLOC       = 2  // Exists in memory
    };

    Optional<size_t> load_elf_file(PageFrame& user_frame, const size_t address);
    void add_elf_from_module(const memory::MultibootInfo& info, const char* name);
}
