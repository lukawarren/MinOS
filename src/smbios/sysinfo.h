#ifndef SYSINFO_H
#define SYSINFO_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct SMBIOSEntryPoint
{
 	int8_t EntryPointString[4];    //This is _SM_
 	uint8_t Checksum;              //This value summed with all the values of the table, should be 0 (overflow)
 	uint8_t Length;                //Length of the Entry Point Table. Since version 2.1 of SMBIOS, this is 0x1F
 	uint8_t MajorVersion;          //Major Version of SMBIOS
 	uint8_t MinorVersion;          //Minor Version of SMBIOS
 	uint16_t MaxStructureSize;     //Maximum size of a SMBIOS Structure (we will se later)
 	uint8_t EntryPointRevision;    //...
 	int8_t FormattedArea[5];       //...
 	int8_t EntryPointString2[5];   //This is _DMI_
 	uint8_t Checksum2;             //Checksum for values from EntryPointString2 to the end of table
 	uint16_t TableLength;          //Length of the Table containing all the structures
 	uint32_t TableAddress;	     //Address of the Table
 	uint16_t NumberOfStructures;   //Number of structures in the table
    uint8_t BCDRevision;           //Unused
} Data;

typedef struct SMBIOSHeader
{
    uint8_t Type;
    uint8_t Length;
    uint8_t Handle;
} Header;

typedef struct _BIOSTable
{
    char Vendor[8];
    char Version[8];
    uint32_t StartingAddressSegment; // A word, so only 32 because we're 32-bit
    char ReleaseDate[8];
    char RomSize[8];

} BIOSTable;

void initSMBIOS();
void printSMBIOS();

#endif
