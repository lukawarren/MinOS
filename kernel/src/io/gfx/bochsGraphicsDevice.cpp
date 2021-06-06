#include "io/gfx/bochsGraphicsDevice.h"
#include "memory/memory.h"
#include "cpu/cpu.h"
#include "kstdlib.h"

namespace Framebuffer
{
    BochsGraphicsDevice::BochsGraphicsDevice(const PCI::Device& device) : GraphicsDevice()
    {
        const uint16_t width = 1600;
        const uint16_t height = 900;

        // Get address from PCI device BAR 0
        m_Address = device.bars[0] & 0xfffffff0;

        // Set width and height as we please
        SetVideoMode(width, height, 32, true, true);
    }

    void BochsGraphicsDevice::WriteReigster(const uint16_t index, const uint16_t data)
    {
        CPU::outw(VBE_DISPI_IOPORT_INDEX, index);
        CPU::outw(VBE_DISPI_IOPORT_DATA, data);
    }
    
    uint16_t BochsGraphicsDevice::ReadRegister(uint16_t index)
    {
        CPU::outw(VBE_DISPI_IOPORT_INDEX, index);
        return CPU::inw(VBE_DISPI_IOPORT_DATA);
    }
    
    void BochsGraphicsDevice::SetVideoMode(const uint16_t width, const uint16_t height, const uint16_t depth, const bool bUseLinearFramebuffer, const bool bClearVideoMemory)
    {
        WriteReigster(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);
        WriteReigster(VBE_DISPI_INDEX_XRES, width);
        WriteReigster(VBE_DISPI_INDEX_YRES, height);
        WriteReigster(VBE_DISPI_INDEX_BPP, depth);
        WriteReigster
        (
            VBE_DISPI_INDEX_ENABLE,
            uint16_t
            ( 
                VBE_DISPI_ENABLED |
                (bUseLinearFramebuffer ? VBE_DISPI_LFB_ENABLED : 0) |
                (bClearVideoMemory ? 0 : VBE_DISPI_NOCLEARMEM)
            )
        );

        m_Width = width;
        m_Height = height;
        m_Pitch = sizeof(uint32_t)*m_Width;
        m_Size = m_Pitch*m_Height;
    }
}