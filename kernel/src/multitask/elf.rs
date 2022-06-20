// See https://en.wikipedia.org/wiki/Executable_and_Linkable_Format

use crate::memory::allocator::PageAllocator;
use crate::memory::paging::PageFrame;
use core::fmt::Error;
use core::mem;
use core::ptr;
use bitflags::bitflags;

const ELF_MAGIC_HEADER: [u8; 4] = [0x7f, b'E', b'L', b'F'];

// ELF types
type Elf32Half = u16;
type Elf32Offset = u32;
type Elf32Address = u32;
type Elf32Word = u32;
type Elf32SignedWord = i32;

#[repr(C, packed)]
struct ElfHeader
{
    e_ident:        [u8; 16],
    e_type:         ElfType,
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

// For e_ident
const ELF_CLASS_32: u8 = 1;
const ELF_DATA_2_LSB: u8 = 1;
const EV_CURRENT: u8 = 1;
const EM_386: Elf32Half = 3;

// For e_type
#[derive(PartialEq, Debug)]
enum ElfType
{
    ETNone  = 0, // unknown
    ETRel   = 1, // relocatable file
    ETExec  = 2  // executable file
}

// For p_type
#[repr(u32)]
#[derive(PartialEq, Copy, Clone)]
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
bitflags!
{
    struct SectionHeaderFlags : Elf32Word
    {
        const SHF_WRITE = 1;  // writable section
        const SHF_ALLOC = 2;  // exists in memory
    }
}

#[repr(C, packed)]
struct ElfSectionHeader
{
    sh_name:        Elf32Word,
    sh_type:        Elf32Word,
    sh_flags:       SectionHeaderFlags,
    sh_addr:        Elf32Address,
    sh_offset:      Elf32Offset,
    sh_size:        Elf32Word,
    sh_link:        Elf32Word,
    sh_info:        Elf32Word,
    sh_addralign:   Elf32Word,
    sh_entsize:     Elf32Word
}

pub unsafe fn load_elf_file(address: usize, allocator: &mut PageAllocator, frame: &mut PageFrame) -> usize
{
    // Get ELF header
    let header = &*(address as *const ElfHeader);

    // Check for magic header
    assert_eq!(header.e_ident[ElfIdent::EIMag0 as usize],       ELF_MAGIC_HEADER[0]);
    assert_eq!(header.e_ident[ElfIdent::EIMag1 as usize],       ELF_MAGIC_HEADER[1]);
    assert_eq!(header.e_ident[ElfIdent::EIMag2 as usize],       ELF_MAGIC_HEADER[2]);
    assert_eq!(header.e_ident[ElfIdent::EIMag3 as usize],       ELF_MAGIC_HEADER[3]);

    // Check it's compatible
    let header_e_machine = header.e_machine;
    assert_eq!(header.e_ident[ElfIdent::EIClass as usize],      ELF_CLASS_32);
    assert_eq!(header.e_ident[ElfIdent::EIData as usize],       ELF_DATA_2_LSB);
    assert_eq!(header.e_ident[ElfIdent::EIVersion as usize],    EV_CURRENT);
    assert_eq!(header_e_machine,                                EM_386);
    assert_eq!(header.e_type,                                   ElfType::ETExec);

    // Load program headers
    for i in 0..header.e_phnum as usize
    {
        let addr = address + (header.e_phoff as usize) + mem::size_of::<ElfProgramHeader>() * i;
        let program_header = &*(addr as *const ElfProgramHeader);
        let program_type = program_header.p_type;

        if program_type == ProgramHeaderType::PT_LOAD
        {
            let source = address + program_header.p_offset as usize;
            let dest = allocator.allocate_user_page_with_address(program_header.p_vaddr as usize, frame);
            ptr::copy_nonoverlapping(source as *mut u8, dest as *mut u8, program_header.p_memsz as usize);
        }
    }

    // Load section headers
    for i in 0..header.e_shnum as usize
    {
        let addr = address + (header.e_shoff as usize) + mem::size_of::<ElfSectionHeader>() * i;
        let section_header = &*(addr as *const ElfSectionHeader);
        let flags = section_header.sh_flags;

        if section_header.sh_size > 0 && flags.contains(SectionHeaderFlags::SHF_ALLOC)
        {
            match section_header.sh_type
            {
                _ => todo!()
            }
        }

        else if flags.contains(SectionHeaderFlags::SHF_WRITE) { todo!(); }
    }

    // Return entrypoint
    header.e_entry as usize
}