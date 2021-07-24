#pragma once
#ifndef BOCHS_GRAPHICS_DEVICE
#define BOCHS_GRAPHICS_DEVICE

#include <stdint.h>
#include <stddef.h>

#include "io/gfx/graphicsDevice.h"
#include "io/pci.h"

// See Bochs's vga.h for defines

#define VBE_DISPI_INDEX_ID          0
#define VBE_DISPI_INDEX_XRES        1
#define VBE_DISPI_INDEX_YRES        2
#define VBE_DISPI_INDEX_BPP         3
#define VBE_DISPI_INDEX_ENABLE      4
#define VBE_DISPI_INDEX_BANK        5
#define VBE_DISPI_INDEX_VIRT_WIDTH  6
#define VBE_DISPI_INDEX_VIRT_HEIGHT 7
#define VBE_DISPI_INDEX_X_OFFSET    8
#define VBE_DISPI_INDEX_Y_OFFSET    9

#define VBE_DISPI_IOPORT_INDEX      0x01CE
#define VBE_DISPI_IOPORT_DATA       0x01CF

#define VBE_DISPI_DISABLED          0
#define VBE_DISPI_INDEX_ENABLE      4
#define VBE_DISPI_ENABLED           1

#define VBE_DISPI_LFB_ENABLED       0x40
#define VBE_DISPI_NOCLEARMEM        0x80

#define VBE_DISPI_ID0                   0xB0C0
#define VBE_DISPI_ID1                   0xB0C1
#define VBE_DISPI_ID2                   0xB0C2
#define VBE_DISPI_ID3                   0xB0C3
#define VBE_DISPI_ID4                   0xB0C4

namespace Framebuffer
{
    class BochsGraphicsDevice : public GraphicsDevice
    {
    public:
        BochsGraphicsDevice(const PCI::Device& device);
        ~BochsGraphicsDevice() {}

        void WriteReigster(const uint16_t index, const uint16_t data);
        uint16_t ReadRegister(const uint16_t index);

        void SetVideoMode(const uint16_t width, const uint16_t height, const uint16_t depth, const bool bUseLinearFramebuffer, const bool bClearVideoMemory);
        void SetWidthAndHeight(const uint32_t width, const uint32_t height);

        void SwapBuffers() override;

    private:
        bool m_bUsingFirstBuffer;

        void UseFirstBuffer();
        void UseSecondBuffer();
    };
}

#endif