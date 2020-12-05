#pragma once
#ifndef MMU_H
#define MMU_H

#define PD_PRESENT(x)           ((x & 0b1))
#define PD_READWRITE(x)         ((x & 0b1) << 1)
#define PD_GLOBALACCESS(x)      ((x & 0b1) << 2)

extern "C"
{
    extern void LoadPageDirectories(uint32_t pageDirectoryAddr);
    extern void EnablePaging();
    extern void FlushTLB();
}

#endif