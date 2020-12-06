#include "elf.h"
#include "../gfx/vga.h"
#include "../memory/paging.h"

static void LoadElfSegment(void* file, ElfProgramHeader* programHeader, void* memory);

void* LoadElfFile(void* file)
{
    // Get ELF header
    Elf32Header* header = (Elf32Header*)file;
    // Check for magic number
    if (header->e_ident[EI_MAG0] != ELFMAG0) { VGA_printf("Incorrect ELF mag0"); return NULL; }
    if (header->e_ident[EI_MAG1] != ELFMAG1) { VGA_printf("Incorrect ELF mag1"); return NULL; }
    if (header->e_ident[EI_MAG2] != ELFMAG2) { VGA_printf("Incorrect ELF mag2"); return NULL; }
    if (header->e_ident[EI_MAG3] != ELFMAG3) { VGA_printf("Incorrect ELF mag3"); return NULL; }
    
    // Check other header values
    if (header->e_ident[EI_CLASS]   != ELFCLASS32)    { VGA_printf("Incorrect ELF class");        return NULL; }
    if (header->e_ident[EI_DATA]    != ELFDATA2LSB)   { VGA_printf("Incorrect ELF byte order");   return NULL; }
    if (header->e_ident[EI_VERSION] != EV_CURRENT)    { VGA_printf("Incorrect ELF version");      return NULL; }
    if (header->e_machine           != EM_386)        { VGA_printf("Incorrect ELF target");       return NULL; }
    if (header->e_type              != ET_EXEC)       { VGA_printf("Incorrect ELF type");         return NULL; }

    // Get total file size for malloc
    ElfProgramHeader* programHeader = (ElfProgramHeader*)((uint32_t)file + header->e_phoff);
    uint32_t fileSize = 0;
    for (uint32_t i = 0; i < header->e_phnum; ++i)
    {
        if (programHeader[i].p_type == PT_LOAD)
        {
            fileSize += programHeader[i].p_memsz;
        }
    }

    // Allocate memory - not writable for now
    void* memory = malloc(fileSize, PD_PRESENT(1) | PD_READWRITE(0) | PD_GLOBALACCESS(1));

    // Load program headers and look for loadable sections
    for (uint32_t i = 0; i < header->e_phnum; ++i)
    {
        if (programHeader[i].p_type == PT_LOAD)
        {
            LoadElfSegment(file, &programHeader[i], memory);
        }
    }

    return (void*)((uint32_t)memory + header->e_entry);
}

static void LoadElfSegment(void* file, ElfProgramHeader* programHeader, void* memory)
{
    uint32_t memorySize     = programHeader->p_memsz;   // Size in memory
    uint32_t fileSize       = programHeader->p_filesz;  // Size in file
    uint32_t memoryPosition = programHeader->p_vaddr;   // Offset in memory
    uint32_t filePosition   = programHeader->p_offset;  // Offset in file

    // Minus the 0x40000000 from memory position
    memoryPosition -= 0x40000000;

    // Get flags
    uint32_t flags = PD_PRESENT(1) | PD_READWRITE(0) | PD_GLOBALACCESS(1);
    if (programHeader->p_flags & ELF_PT_W) flags |= PD_READWRITE(1);

    // Set segment's page to have appropriate flags
    // WARNING: code assumes alignment on page boundary!
    AllocatePage((uint32_t)memory + memoryPosition, (uint32_t)memory + memoryPosition, flags, false);

    // Load segment
    uint32_t segmentMemory  = (uint32_t)memory + memoryPosition;
    memcpy((void*)segmentMemory, (void*)((uint32_t)file + filePosition), memorySize);
    memset((void*)((uint32_t)segmentMemory + fileSize), 0, memorySize - fileSize);
}