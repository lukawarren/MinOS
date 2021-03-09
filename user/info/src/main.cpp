#include "interrupts/syscall.h"
#include "memory/gdt.h"
#include "stdlib.h"

int main();

// Buffer
char* buffer;
uint32_t bufferCounter = 0;

// Printing utils
void Print(const char* string);
void Printn(const uint32_t data, const uint32_t width);
void AddColumns(const int nColmuns);

void Print(const char* string)
{ 
    for (size_t i = 0; i < strlen(string); ++i)
    {
        buffer[bufferCounter] = string[i];
        buffer[bufferCounter + 1] = '\0';
        bufferCounter++;
    }
}
void Printn(const uint32_t data, const uint32_t width)
{
    auto hexToASCII = [](const size_t number) 
    {
        char value = number % 16 + 48;
        if (value > 57) value += 7;
        return value;
    };
    auto getNthDigit = [](const size_t number, const size_t digit, const size_t base) { return int((number / pow(base, digit)) % base); };

    Print("0x");
    for (size_t d = 0; d < width; ++d)
    { 
        buffer[bufferCounter] = hexToASCII(getNthDigit(data, width - d - 1, 16));
        buffer[bufferCounter + 1] = '\0';
        bufferCounter++;
    }
}

void AddColumns(const int nColmuns) { for (int i = 0; i < nColmuns; ++i) Print(" "); }

// Functions
void PrintGDT();
void PrintMemory();

int main()
{
    printf("\n");
    PrintGDT();
    printf("\n");
    PrintMemory();
    printf("\n");

    sysexit();
    return 0;
}

void PrintGDT()
{
    buffer = (char*)malloc(4096);

    // Get GDT info
    uint64_t gdtEntries[6];
    getGDT(&gdtEntries);

    Print("Base");
    AddColumns(10);
    Print("Limit");
    AddColumns(10);
    Print("DPL");
    AddColumns(10);
    Print("Type");
    AddColumns(14);
    Print("Selector\n");

    for (unsigned int i = 0; i < sizeof(gdtEntries) / sizeof(gdtEntries[0]); ++i)
    {
        uint64_t descriptor = gdtEntries[i];

        // Base adresss
        uint32_t baseAddress = (uint32_t)(descriptor >> 16) & 0xFF; // Bits 0:15
        baseAddress |= (uint32_t)(descriptor >> 16) & 0xFF00;       // Bits 16:23
        baseAddress |= (uint32_t)(descriptor >> 16) & 0xFF0000;     // Bits 24:31
        Printn(baseAddress, 8);

        AddColumns(4);

        // Limit
        uint32_t limit = (uint32_t)(descriptor & 0xFFFF);   // Bits 0:15
        limit |= (uint32_t)(descriptor >> 32) & 0xF0000;    // Bits 16:19
        Printn(limit, 8);

        AddColumns(5);
        
        // Get access byte and flags
        uint32_t flags = (uint32_t)(descriptor >> 40) & 0xF0FF;

        // DPL
        uint8_t DPL = (flags & 0b01100000) >> 5;
        Printn(DPL, 2);

        AddColumns(9);

        // Type 
        if (((flags >> 0x4) & 1) == (SEG_DESCTYPE(0)))
        {
            if (((flags >> 0x7) & 1) == (SEG_PRES(0))) Print("Unused     ");
            else Print("TSS        ");
        }
        else
        {
            if (flags & 0b1000) Print("32-bit code");
            else Print("32-bit data");
        }

        AddColumns(7);

        // Selector
        Printn(i * 8, 2);
        Print("\n");
    }

    printf(buffer);
    free(buffer, 4096);
}

void PrintMemory()
{
    printf("Memory usage: ");
    printn(nPages() * 4096 / 1024 / 1024, false);
    printf("MB out of ");
    printn(nTotalPages() * 4096 / 1024 / 1024, false);
    printf("MB --- ");
    printn(nPages() * 100 / nTotalPages(), false);
    printf("%\n");
}
