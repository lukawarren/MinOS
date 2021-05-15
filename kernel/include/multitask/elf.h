#pragma once
#ifndef ELF_H
#define ELF_H

#include <stdint.h>
#include <stddef.h>

#include "memory/pageFrame.h"

// See https://en.wikipedia.org/wiki/Executable_and_Linkable_Format

// e_ident data
#define ELFMAG0	0x7F    // e_ident[EI_MAG0]
#define ELFMAG1	'E'     // e_ident[EI_MAG1]
#define ELFMAG2	'L'     // e_ident[EI_MAG2]
#define ELFMAG3	'F'     // e_ident[EI_MAG3]
#define ELFDATA2LSB	1   // Little Endian
#define ELFCLASS32	1   // 32-bit Architecture

// Type
# define EM_386		(3)  // x86 Machine Type
# define EV_CURRENT	(1)  // ELF Current Version

namespace Multitask
{
    // Elf file types
    typedef uint16_t elf32_half;	    // Unsigned half int
    typedef uint32_t elf32_offset;	    // Unsigned offset
    typedef uint32_t elf32_address;	    // Unsigned address
    typedef uint32_t elf32_word;	    // Unsigned int
    typedef int32_t  elf32_signedWord;	// Signed int

    // Elf header
    struct Elf32Header
    {
        uint8_t         e_ident[16];
        elf32_half	    e_type;
        elf32_half	    e_machine;
        elf32_word	    e_version;
        elf32_address	e_entry;
        elf32_offset	e_phoff;
        elf32_offset	e_shoff;
        elf32_word	    e_flags;
        elf32_half	    e_ehsize;
        elf32_half	    e_phentsize;
        elf32_half	    e_phnum;
        elf32_half	    e_shentsize;
        elf32_half	    e_shnum;
        elf32_half	    e_shstrndx;
    } __attribute__((packed));

    // Elf e_ident
    enum ElfIdent
    {
        EI_MAG0		    = 0, // 0x7F
        EI_MAG1		    = 1, // 'E'
        EI_MAG2		    = 2, // 'L'
        EI_MAG3		    = 3, // 'F'
        EI_CLASS	    = 4, // Architecture (32/64)
        EI_DATA		    = 5, // Byte Order
        EI_VERSION	    = 6, // ELF Version
        EI_OSABI	    = 7, // OS Specific
        EI_ABIVERSION	= 8, // OS Specific
        EI_PAD		    = 9  // Padding
    };

    enum ElfType
    {
        ET_NONE     = 0,    // Unknown
        ET_REL      = 1,    // Relocatable file
        ET_EXEC    = 2      // Executable file
    };

    // Program header
    struct ElfProgramHeader
    {
        elf32_word p_type;
        elf32_word p_offset;
        elf32_address p_vaddr;
        elf32_address p_paddr;
        elf32_word p_filesz;
        elf32_word p_memsz;
        elf32_address p_flags;
        elf32_address p_align;
    } __attribute__((packed));

    // Program header p_type
    enum ProgramHeaderType
    {
        PT_NULL = 0,    // Unused
        PT_LOAD = 1,    // Loadable segment
    };

    uint32_t LoadElfProgram(uint32_t pFile, Memory::PageFrame& pageFrame);
}

#endif