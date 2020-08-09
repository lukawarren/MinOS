#pragma once
#ifndef MMU_H
#define MMU_H

#define PD_PRESENT(x)       ((x & 0b1))
#define PD_READWRITE(x)     ((x & 0b1) << 1)
#define PD_SUPERVISOR(x)    ((x & 0b0) << 2)

extern "C"
{
    extern void LoadPageDirectory(uint32_t pageDirectoryAddr);
    extern void EnablePaging();
}

#endif