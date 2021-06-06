#include "io/pci.h"
#include "io/uart.h"
#include "cpu/cpu.h"
#include "memory/memory.h"
#include "kstdlib.h"

namespace PCI
{
    Device devices[32];
    uint32_t nDevices = 0;

    void Init()
    {
        // Probe the PCI bus with brute-force
        for (uint16_t bus = 0; bus < 256; ++bus)
        {
            for (uint8_t slot = 0; slot < 32; ++slot)
            {
                // If device is valid
                if (GetVendor((uint8_t)bus, slot) != PCI_VENDOR_INVALID)
                {
                    // Get header type
                    const auto headerType = GetHeaderType((uint8_t)bus, slot);

                    // Assert header is type 0 (straight forward device), with a single function
                    assert(headerType == 0);

                    // Get class
                    devices[nDevices].vendorID = GetVendor((uint8_t)bus, slot);
                    devices[nDevices].classCode = GetClassCode((uint8_t)bus, slot);
                    devices[nDevices].subclass = GetSubclass((uint8_t)bus, slot);

                    // Get BARs
                    for (uint8_t i = 0; i < 6; ++i)
                        devices[nDevices].bars[i] = GetNthBar((uint8_t)bus, slot, i);
                    
                    assert(nDevices <= 31);
                    nDevices++;
                }
            }
        }

        UART::WriteString("[PCI] Scan finished\n");
    }
    
    uint32_t ReadFromConfig(const uint8_t bus, const uint8_t slot, const uint8_t function, const uint8_t offset)
    {
        /*
            Use "configuration space access mechanism #1"

            | 31	        |   30 - 24     |   23 - 16	     |  15 - 11	      | 10 - 8	         |  7 - 0           |
            | --------------|---------------|----------------|----------------|------------------|----------------- |
            | Enable Bit	|   Reserved	|   Bus Number	 |  Device Number | Function Number  |  Register Offset |
        */

        // Create configuratiion address
        const uint32_t address = (uint32_t)
        (
            (uint32_t)(bus << 16)       |
            (uint32_t)(slot << 11)      |
            (uint32_t)(function << 8)   |
            (offset & 0xfc)             |
            (0x80000000)
        );

        // Write to address than read in data
        CPU::outl(PCI_CONFIG_ADDRESS, address);
        return CPU::inl(PCI_CONFIG_DATA);
    }

    uint16_t GetVendor(const uint8_t bus, const uint8_t slot)
    {
        return (uint16_t) (ReadFromConfig(bus, slot, 0, PCI_HEADER_VENDOR_OFFSET) >> 16); // lower 16 bits
    }

    uint8_t GetHeaderType(const uint8_t bus, const uint8_t slot)
    {
        return (uint8_t) ReadFromConfig(bus, slot, 0, PCI_HEADER_HEADER_TYPE); // lower 8 bits
    }

    uint8_t GetClassCode(const uint8_t bus, const uint8_t slot)
    {
        return (uint8_t) (ReadFromConfig(bus, slot, 0, PCI_HEADER_CLASS_CODE) >> 24); // upper 8 bits
    }

    uint8_t GetSubclass(const uint8_t bus, const uint8_t slot)
    {
        return (uint8_t)(ReadFromConfig(bus, slot, 0, PCI_HEADER_HEADER_TYPE) & 0xff); // lower 8 bits
    }

    uint32_t GetNthBar(const uint8_t bus, const uint8_t slot, const uint8_t bar)
    {
        return ReadFromConfig(bus, slot, 0, uint8_t(PCI_HEADER_BAR_0 + bar*4));
    }

}
