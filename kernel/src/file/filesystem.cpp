#include "filesystem.h"
#include "../gfx/vga.h"

void ParseVFS(uint32_t vfs)
{
    File* file = (File*)vfs;
    VGA_printf(file->sName);
}