#include "io/gfx/framebuffer.h"
#include "io/gfx/bochsGraphicsDevice.h"
#include "io/gfx/genericGraphicsDevice.h"
#include "io/uart.h"
#include "io/pci.h"
#include "memory/pageFrame.h"
#include "filesystem/filesystem.h"
#include "filesystem/deviceFile.h"
#include "kstdlib.h"

namespace Framebuffer
{
    GraphicsDevice* graphicsDevice;

    void Init(const multiboot_info_t* pMultiboot)
    {
        bool bFoundBochsDevice = false;

        // Search for Bochs VGA card 
        for (const auto device : PCI::devices)
        {
            if (device.classCode == PCI_CLASS_DISPLAY_CONTROLLER && 
                ((device.vendorID == 0x1234 && device.deviceID == 0x1111) ||
                (device.vendorID == 0x80ee && device.deviceID == 0xbeef)))
            {
                UART::WriteString("[PCI] Bochs display controller found\n");
                graphicsDevice = new BochsGraphicsDevice(device);
                bFoundBochsDevice = true;
            }
        }

        if (!bFoundBochsDevice) graphicsDevice = new GenericGraphicsDevice(pMultiboot);

        // Install file
        *Filesystem::GetFile(Filesystem::FileDescriptors::framebuffer) = Filesystem::DeviceFile(graphicsDevice->m_Size, (void*)graphicsDevice->m_Address, "/dev/fb", Filesystem::FileDescriptors::framebuffer);
    
        UART::WriteString("[Framebuffer] Initialised\n");
    }

    uint32_t GetColour(const uint8_t r, const uint8_t g, const uint8_t b)
    {
        const uint32_t a = 0xff;
        return a << 24 | r << 16 | g << 8 | b;
    }
}