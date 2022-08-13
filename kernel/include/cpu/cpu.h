namespace CPU
{
    inline void outb(uint16_t port, uint8_t data)
    {
        asm volatile ( "outb %0, %1" : : "a"(data), "d"(port) );
    }

    inline uint8_t inb(uint16_t port)
    {
        uint8_t ret;
        asm volatile ( "inb %1, %0"
                    : "=a"(ret)
                    : "Nd"(port) );
        return ret;
    }

    inline void outw(uint16_t port, uint16_t data)
    {
        asm volatile ( "out %0, %1" : : "a"(data), "d"(port) );
    }

    inline uint16_t inw(uint16_t port)
    {
        uint16_t ret;
        asm volatile ( "in %1, %0"
                    : "=a"(ret)
                    : "Nd"(port) );
        return ret;
    }

    inline void outl(uint16_t port, uint32_t data)
    {
        asm volatile ( "outl %0, %1" : : "a"(data), "d"(port) );
    }

    inline uint32_t inl(uint16_t port)
    {
        uint32_t ret;
        asm volatile ( "inl %1, %0"
                    : "=a"(ret)
                    : "Nd"(port) );
        return ret;
    }
}