#include "elf.h"
#include "../gfx/vga.h"
#include "../memory/paging.h"

#define ELF_ERROR_MESSAGE(message) VGA_printf("[Failure] ", false, VGA_COLOUR_LIGHT_RED); VGA_printf(message);

static void LoadElfSegment(void* file, ElfProgramHeader* programHeader, void* memory);

ElfReturn LoadElfFile(void* file)
{
    // Get ELF header
    Elf32Header* header = (Elf32Header*)file;
    // Check for magic number
    if (header->e_ident[EI_MAG0] != ELFMAG0) { ELF_ERROR_MESSAGE("Incorrect ELF mag0"); return {}; }
    if (header->e_ident[EI_MAG1] != ELFMAG1) { ELF_ERROR_MESSAGE("Incorrect ELF mag1"); return {}; }
    if (header->e_ident[EI_MAG2] != ELFMAG2) { ELF_ERROR_MESSAGE("Incorrect ELF mag2"); return {}; }
    if (header->e_ident[EI_MAG3] != ELFMAG3) { ELF_ERROR_MESSAGE("Incorrect ELF mag3"); return {}; }
    
    // Check other header values
    if (header->e_ident[EI_CLASS]   != ELFCLASS32)    { ELF_ERROR_MESSAGE("Incorrect ELF class");        return {}; }
    if (header->e_ident[EI_DATA]    != ELFDATA2LSB)   { ELF_ERROR_MESSAGE("Incorrect ELF byte order");   return {}; }
    if (header->e_ident[EI_VERSION] != EV_CURRENT)    { ELF_ERROR_MESSAGE("Incorrect ELF version");      return {}; }
    if (header->e_machine           != EM_386)        { ELF_ERROR_MESSAGE("Incorrect ELF target");       return {}; }
    if (header->e_type              != ET_EXEC)       { ELF_ERROR_MESSAGE("Incorrect ELF type");         return {}; }

    // Get total file size for malloc
    ElfProgramHeader* programHeader = (ElfProgramHeader*)((uint32_t)file + header->e_phoff);
    uint32_t fileSize = 0;
    for (uint32_t i = 0; i < header->e_phnum; ++i)
    {
        if (programHeader[i].p_type == PT_LOAD)
        {
            uint32_t newFileSize = (programHeader[i].p_vaddr - 0x40000000) + programHeader[i].p_memsz;
            if (newFileSize > fileSize) fileSize = newFileSize;
        }
    }

    // Allocate memory - not writable for now
    void* memory = malloc(fileSize, USER_PAGE); // Really should be (see LoadElfSegment) - PD_PRESENT(1) | PD_READWRITE(0) | PD_GLOBALACCESS(1);

    // Load program headers and look for loadable sections
    for (uint32_t i = 0; i < header->e_phnum; ++i)
    {
        if (programHeader[i].p_type == PT_LOAD)
        {
            LoadElfSegment(file, &programHeader[i], memory);
        }
    }

    return { header->e_entry, fileSize, (uint32_t) memory };
}

static void LoadElfSegment(void* file, ElfProgramHeader* programHeader, void* memory)
{
    uint32_t memorySize     = programHeader->p_memsz;   // Size in memory
    uint32_t fileSize       = programHeader->p_filesz;  // Size in file
    uint32_t memoryPosition = programHeader->p_vaddr;   // Offset in memory
    uint32_t filePosition   = programHeader->p_offset;  // Offset in file

    // Minus the 0x40000000 from memory position
    memoryPosition -= 0x40000000;

    // Get and set segment's page to have appropriate flags
    // WARNING: code assumes alignment on page boundary!
    // ...just kidding, it complicates multitask.cpp
    //uint32_t flags = PD_PRESENT(1) | PD_READWRITE(0) | PD_GLOBALACCESS(1);
    //if (programHeader->p_flags & ELF_PT_W) flags |= PD_READWRITE(1);
    //AllocatePage((uint32_t)memory + memoryPosition, (uint32_t)memory + memoryPosition, flags, false);
    
    // Load segment
    uint32_t segmentMemory  = (uint32_t)memory + memoryPosition;
    memcpy((void*)segmentMemory, (void*)((uint32_t)file + filePosition), memorySize);
    memset((void*)((uint32_t)segmentMemory + fileSize), 0, memorySize - fileSize);
}