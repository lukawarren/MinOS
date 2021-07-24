#include "io/gfx/genericGraphicsDevice.h"
#include "memory/memory.h"
#include "kstdlib.h"

namespace Framebuffer
{
    GenericGraphicsDevice::GenericGraphicsDevice(const multiboot_info_t* pMultiboot) : GraphicsDevice()
    {
        // Get address and check we're page aligned
        m_Address = (size_t) pMultiboot->framebuffer_addr;
        assert(m_Address % PAGE_SIZE == 0);

        // Work out size
        m_Width = pMultiboot->framebuffer_width;
        m_Height = pMultiboot->framebuffer_height;
        m_Pitch = pMultiboot->framebuffer_pitch;
        m_Size = m_Pitch * m_Width;
        assert(m_Pitch == sizeof(uint32_t)*m_Width); // Check it's 1 uint32_t per pixel

        // Map framebuffer into kernel
        const uint32_t nPages = Memory::RoundToNextPageSize(m_Size) / PAGE_SIZE;
        for (uint32_t i = 0; i < nPages; ++i)
            Memory::kPageFrame.SetPage(m_Address + i*PAGE_SIZE, m_Address + i*PAGE_SIZE, KERNEL_PAGE);
    }
}