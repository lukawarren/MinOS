#pragma once
#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>
#include <stddef.h>

#define PAGE_SIZE 0x1000

extern uint32_t __kernel_end;

struct PageFlags
{
    uint8_t allocated: 1 ;
    uint8_t kernel: 1;
    uint8_t padding: 6;
};

struct Page
{
    uint32_t virtualAddress;
    PageFlags flags;
};

void InitPaging(uint32_t maxAddress);

#endif