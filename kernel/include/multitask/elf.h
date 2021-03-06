#pragma once
#ifndef ELF_H
#define ELF_H

#include <stdint.h>
#include <stddef.h>

/*
    See https://en.wikipedia.org/wiki/Executable_and_Linkable_Format
*/

// Elf file types
typedef uint16_t elf32_half;	    // Unsigned half int
typedef uint32_t elf32_offset;	    // Unsigned offset
typedef uint32_t elf32_address;	    // Unsigned address
typedef uint32_t elf32_word;	    // Unsigned int
typedef int32_t  elf32_signedWord;	// Signed int

// Elf header
# define ELF_NIDENT	16

struct Elf32Header
{
    uint8_t         e_ident[ELF_NIDENT];
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

// e_ident

# define ELFMAG0	0x7F // e_ident[EI_MAG0]
# define ELFMAG1	'E'  // e_ident[EI_MAG1]
# define ELFMAG2	'L'  // e_ident[EI_MAG2]
# define ELFMAG3	'F'  // e_ident[EI_MAG3]
 
# define ELFDATA2LSB	(1)  // Little Endian
# define ELFCLASS32	(1)  // 32-bit Architecture

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

// Type
# define EM_386		(3)  // x86 Machine Type
# define EV_CURRENT	(1)  // ELF Current Version

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

#define ELF_PT_X 0x1
#define ELF_PT_R 0x2
#define ELF_PT_W 0x4

struct ElfReturn
{
	uint32_t entry;
	uint32_t size;
	uint32_t location;
	uint32_t error = 0;
	
	ElfReturn(uint32_t _entry, uint32_t _size, uint32_t _location) : entry(_entry), size(_size), location(_location), error(0) {}
	ElfReturn() : error(1) {}
};
ElfReturn LoadElfFile(void* file);

#endif