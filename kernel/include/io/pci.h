#pragma once
#ifndef PCI_H
#define PCI_H

#include <stdint.h>
#include <stddef.h>

#define PCI_CONFIG_ADDRESS          0xCF8
#define PCI_CONFIG_DATA             0xCFC

// Headers
#define PCI_HEADER_DEVICE_OFFSET    0
#define PCI_HEADER_VENDOR_OFFSET    2
#define PCI_VENDOR_INVALID          0xffff
#define PCI_HEADER_HEADER_TYPE      0xc
#define PCI_HEADER_CLASS_CODE       0x8
#define PCI_HEADER_SUBCLASS         0x8
#define PCI_HEADER_BAR_0            0x10

// Class codes
#define PCI_CLASS_UNCLASSIFIED          0
#define PCI_CLASS_MASS_STORAGE          1
#define PCI_CLASS_NETWORK_CONTROLLER    2
#define PCI_CLASS_DISPLAY_CONTROLLER    3

namespace PCI
{
    struct Device
    {
        uint16_t vendorID;
        uint8_t classCode;
        uint8_t subclass;
        uint32_t bars[6];
    };

    void Init();
    
    uint32_t ReadFromConfig(const uint8_t bus, const uint8_t slot, const uint8_t function, const uint8_t offset);

    uint16_t GetVendor(const uint8_t bus, const uint8_t slot);
    uint8_t  GetHeaderType(const uint8_t bus, const uint8_t slot);
    uint8_t  GetClassCode(const uint8_t bus, const uint8_t slot);
    uint8_t  GetSubclass(const uint8_t bus, const uint8_t slot);
    uint32_t GetNthBar(const uint8_t bus, const uint8_t slot, const uint8_t bar);

    extern Device devices[32];
    extern uint32_t nDevices;
}

#endif