// See https://en.wikipedia.org/wiki/Executable_and_Linkable_Format

use crate::memory::allocator::PageAllocator;

const ELF_MAGIC_HEADER: [u8; 4] = [0x7f, b'E', b'L', b'F'];

// ELF types
type Elf32Half = u16;
type Elf32Offset = u32;
type Elf32Address = u32;
type Elf32Word = u32;
type Elf32SignedWord = i32;

#[repr(C, packed)]
struct Header
{
    e_ident:        [u8; 16],
    e_type:         Elf32Half,
    e_machine:      Elf32Half,
    e_version:      Elf32Word,
    e_entry:        Elf32Address,
    e_phoff:        Elf32Offset,
    e_shoff:        Elf32Offset,
    e_flags:        Elf32Word,
    e_ehsize:       Elf32Half,
    e_phentsize:    Elf32Half,
    e_phnum:        Elf32Half,
    e_shentsize:    Elf32Half,
    e_shnum:        Elf32Half,
    e_shstrndx:     Elf32Half
}

// For e_ident
enum ElfIdent
{
    EIMag0 = 0,         // 0x7F
    EIMag1 = 1,         // 'E'
    EIMag2 = 2,         // 'L'
    EIMag3 = 3,         // 'F'
    EIClass = 4,        // architecture (32/64)
    EIData = 5,         // byte Order
    EIVersion = 6,      // ELF Version
    EIOSABI = 7,        // OS Specific
    EIABIVersion = 8,   // OS Specific
    EIPad = 9           // padding
}

// For e_type
enum ElfType
{
    ETNone  = 0, // unknown
    ETRel   = 1, // relocatable file
    ETExec  = 2  // executable file
}

// For p_type
#[repr(u32)]
enum ProgramHeaderType
{
    PT_NULL = 0,    // unused
    PT_LOAD = 1     // loadable segment
}

#[repr(C, packed)]
struct ElfProgramHeader
{
    p_type:     ProgramHeaderType,
    p_offset:   Elf32Word,
    p_vaddr:    Elf32Address,
    p_paddr:    Elf32Address,
    p_filesz:   Elf32Address,
    p_memsz:    Elf32Word,
    p_flags:    Elf32Address,
    p_align:    Elf32Address
}

// For sh_type
#[repr(u32)]
enum SectionHeaderType
{
    SHT_NULL	    = 0,  // null section
    SHT_PROGBITS    = 1,  // program information
    SHT_SYMTAB	    = 2,  // symbol table
    SHT_STRTAB	    = 3,  // string table
    SHT_RELA	    = 4,  // relocation (with addend)
    SHT_NOBITS	    = 8,  // not present in file
    SHT_REL		    = 9,  // relocation (no addend)
    SHT_FINI_ARRAY  = 15, // array of destructors
}

// For sh_flags
#[repr(u32)]
enum SectionHeaderFlags
{
    SHF_WRITE = 1,  // Writable section
    SHF_ALLOC = 2   // Exists in memory
}

#[repr(C, packed)]
struct ElfSectionHeader
{
    sh_name:        Elf32Word,
    sh_type:        Elf32Word,
    sh_flags:       Elf32Word,
    sh_addr:        Elf32Address,
    sh_offset:      Elf32Offset,
    sh_size:        Elf32Word,
    sh_link:        Elf32Word,
    sh_info:        Elf32Word,
    sh_addralign:   Elf32Word,
    sh_entsize:     Elf32Word
}

pub unsafe fn load_elf_file(address: usize, allocator: &PageAllocator)
{
    todo!();
}