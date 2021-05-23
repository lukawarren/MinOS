#include "io/framebuffer.h"
#include "memory/pageFrame.h"
#include "filesystem/filesystem.h"
#include "filesystem/deviceFile.h"
#include "kstdlib.h"

namespace Framebuffer
{
    Framebuffer sFramebuffer = {};

    void Init(const multiboot_info_t* pMultiboot)
    {
        // Get address and check we're page aligned
        sFramebuffer.address = (size_t) pMultiboot->framebuffer_addr;
        assert(sFramebuffer.address % PAGE_SIZE == 0);

        // Work out size
        sFramebuffer.width = pMultiboot->framebuffer_width;
        sFramebuffer.height = pMultiboot->framebuffer_height;
        sFramebuffer.pitch = pMultiboot->framebuffer_pitch;
        sFramebuffer.size = sFramebuffer.pitch * sFramebuffer.width;
        assert(sFramebuffer.pitch == sizeof(uint32_t)*sFramebuffer.width); // Check it's 1 uint32_t per pixel

        // Map framebuffer into kernel
        const uint32_t nPages = sFramebuffer.size / PAGE_SIZE;
        for (uint32_t i = 0; i < nPages; ++i)
            Memory::kPageFrame.SetPage(sFramebuffer.address + i*PAGE_SIZE, sFramebuffer.address + i*PAGE_SIZE, KERNEL_PAGE);

        // Install file
        *Filesystem::GetFile(Filesystem::FileDescriptors::framebuffer) = Filesystem::DeviceFile(sFramebuffer.size, (void*)sFramebuffer.address);
    }

    uint32_t GetColour(const uint8_t r, const uint8_t g, const uint8_t b)
    {
        const uint32_t a = 0xff;
        return a << 24 | r << 16 | g << 8 | b;
    }
}