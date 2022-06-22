#![allow(unaligned_references, dead_code)]

use crate::memory::allocator::PageAllocator;
use crate::memory::paging::PageFrame;
use core::mem;
use core::ptr;
use bitflags::bitflags;

// ========== See https://en.wikipedia.org/wiki/Executable_and_Linkable_Format ==========

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
#[repr(u16)]
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
    PtNull = 0, // unused
    PtLoad = 1, // loadable segment
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
#[derive(Debug)]
enum SectionHeaderType
{
    ShtNull	     = 0,  // null section
    ShtProgBits  = 1,  // program information
    ShtSymTab	 = 2,  // symbol table
    ShtStrTab	 = 3,  // string table
    ShtRela	     = 4,  // relocation (with addend)
    ShtNoBits	 = 8,  // not present in file
    ShtRel		 = 9,  // relocation (no addend)
    ShtFiniArray = 15, // array of destructors
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
    sh_type:        SectionHeaderType,
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
    assert_eq!(header.e_ident[ElfIdent::EIClass as usize],      ELF_CLASS_32);
    assert_eq!(header.e_ident[ElfIdent::EIData as usize],       ELF_DATA_2_LSB);
    assert_eq!(header.e_ident[ElfIdent::EIVersion as usize],    EV_CURRENT);
    assert_eq!(header.e_machine,                                EM_386);
    assert_eq!(header.e_type,                                   ElfType::ETExec);

    // Load program headers
    for i in 0..header.e_phnum as usize
    {
        let addr = address + (header.e_phoff as usize) + mem::size_of::<ElfProgramHeader>() * i;
        let program_header = &*(addr as *const ElfProgramHeader);
        let program_type = program_header.p_type;

        if program_type == ProgramHeaderType::PtLoad
        {
            let source = address + program_header.p_offset as usize;
            let file_size = program_header.p_filesz as usize;
            let mem_size = program_header.p_memsz as usize;

            let dest = allocator.allocate_user_raw_with_address(
                program_header.p_vaddr as usize,
                mem_size,
                frame
            );

            // If p_memsz exceeds p_filesz, then the remaining bits are to be cleared with zeros
            ptr::write_bytes(dest as *mut u8, 0, mem_size);
            ptr::copy_nonoverlapping(source as *mut u8, dest as *mut u8, file_size);
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
                // BSS; allocate memory and zero it out
                SectionHeaderType::ShtNoBits =>
                {
                    let dest = allocator.allocate_user_raw_with_address(
                        section_header.sh_addr as usize,
                        section_header.sh_size as usize,
                        frame
                    );

                    ptr::write_bytes(dest as *mut u8, 0, section_header.sh_size as usize);
                },

                SectionHeaderType::ShtProgBits => {
                    // Already loaded by the program headers (luckily)... I think (well it works for now)
                }

                _ => todo!("{:#?}", section_header.sh_type)
            }
        }

        else if flags.contains(SectionHeaderFlags::SHF_WRITE) { todo!(); }
    }

    // Return entrypoint
    header.e_entry as usize
}