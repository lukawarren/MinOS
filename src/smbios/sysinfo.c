#include "sysinfo.h"

#include "../std/stdout.h"

unsigned char* mem;

void initSMBIOS()
{
    // Find entry point
    mem = (unsigned char *) 0xF0000;
    int length, i;
    unsigned char checksum;
    while ((unsigned int) mem < 0x100000)
    {
        if (mem[0] == '_' && mem[1] == 'S' && mem[2] == 'M' && mem[3] == '_')
        {
            length = mem[5];
            checksum = 0;
            for(i = 0; i < length; i++)
            {
                checksum += mem[i];
            }
            if(checksum == 0) break;
        }
        mem += 16;
    }
    if ((unsigned int) mem == 0x100000)
    {
        stdout_setcolor(VGA_COLOR_LIGHT_RED);
        printf("No SMBIOS found!");
        stdout_setcolor(VGA_COLOR_LIGHT_GREEN);
        return;
    }

    printi("Located SMBIOS at address: ");
    printh((unsigned int) mem);
    printf("");
}

void printSMBIOS()
{
    
    //Store data
    Data* data = (Data*) mem;

    // Print version
    printi("SMBIOS Version: ");
    printn(data->MajorVersion);
    printi(".");
    printn(data->MinorVersion);
    printf("");

    // BIOS Tabel
    Header* header = (Header*) data->TableAddress;
    BIOSTable* BIOS = (BIOSTable*) (&header->Type + header->Length);
    printi("BIOS Vendor: ");
    printf((char*) BIOS->Vendor);
    
    // Finish
    //printf("");

}