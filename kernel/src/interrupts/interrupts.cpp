#include "interrupts/interrupts.h"
#include "multitask/scheduler.h"
#include "interrupts/irq.h"
#include "interrupts/pit.h"
#include "interrupts/pic.h"
#include "dev/uart.h"
#include "cpu/idt.h"
#include "cpu/cpu.h"
#include "fs/fs.h"
#include "klib.h"

namespace interrupts
{
    uint8_t keyboard_buffer[keyboard_buffer_size];
    size_t keyboard_buffer_keys = 0;

    void load()
    {
        using namespace cpu;

        // Build empty IDT
        constexpr int offset = 0x20; // PIC has an offset that must be accounted for
        for (size_t i = 0; i < 256; ++i) idt[i] = create_idt_entry((uint32_t) &blank_irq, 0x8, ENABLED_R0_INTERRUPT);

        // Standard interrupts
        idt[offset+0]  = create_idt_entry((uint32_t) irq_0,  0x8, ENABLED_R0_INTERRUPT);
        idt[offset+1]  = create_idt_entry((uint32_t) irq_1,  0x8, ENABLED_R0_INTERRUPT);
        idt[offset+2]  = create_idt_entry((uint32_t) irq_2,  0x8, ENABLED_R0_INTERRUPT);
        idt[offset+3]  = create_idt_entry((uint32_t) irq_3,  0x8, ENABLED_R0_INTERRUPT);
        idt[offset+4]  = create_idt_entry((uint32_t) irq_4,  0x8, ENABLED_R0_INTERRUPT);
        idt[offset+5]  = create_idt_entry((uint32_t) irq_5,  0x8, ENABLED_R0_INTERRUPT);
        idt[offset+6]  = create_idt_entry((uint32_t) irq_6,  0x8, ENABLED_R0_INTERRUPT);
        idt[offset+7]  = create_idt_entry((uint32_t) irq_7,  0x8, ENABLED_R0_INTERRUPT);
        idt[offset+8]  = create_idt_entry((uint32_t) irq_8,  0x8, ENABLED_R0_INTERRUPT);
        idt[offset+9]  = create_idt_entry((uint32_t) irq_9,  0x8, ENABLED_R0_INTERRUPT);
        idt[offset+10] = create_idt_entry((uint32_t) irq_10, 0x8, ENABLED_R0_INTERRUPT);
        idt[offset+11] = create_idt_entry((uint32_t) irq_11, 0x8, ENABLED_R0_INTERRUPT);
        idt[offset+12] = create_idt_entry((uint32_t) irq_12, 0x8, ENABLED_R0_INTERRUPT);
        idt[offset+13] = create_idt_entry((uint32_t) irq_13, 0x8, ENABLED_R0_INTERRUPT);
        idt[offset+14] = create_idt_entry((uint32_t) irq_14, 0x8, ENABLED_R0_INTERRUPT);
        idt[offset+15] = create_idt_entry((uint32_t) irq_15, 0x8, ENABLED_R0_INTERRUPT);

        // Exceptions
        idt[0]   = create_idt_entry((uint32_t) irq_exception_0,   0x8, ENABLED_R0_INTERRUPT);
        idt[1]   = create_idt_entry((uint32_t) irq_exception_1,   0x8, ENABLED_R0_INTERRUPT);
        idt[2]   = create_idt_entry((uint32_t) irq_exception_2,   0x8, ENABLED_R0_INTERRUPT);
        idt[3]   = create_idt_entry((uint32_t) irq_exception_3,   0x8, ENABLED_R0_INTERRUPT);
        idt[4]   = create_idt_entry((uint32_t) irq_exception_4,   0x8, ENABLED_R0_INTERRUPT);
        idt[5]   = create_idt_entry((uint32_t) irq_exception_5,   0x8, ENABLED_R0_INTERRUPT);
        idt[6]   = create_idt_entry((uint32_t) irq_exception_6,   0x8, ENABLED_R0_INTERRUPT);
        idt[7]   = create_idt_entry((uint32_t) irq_exception_7,   0x8, ENABLED_R0_INTERRUPT);
        idt[8]   = create_idt_entry((uint32_t) irq_exception_8,   0x8, ENABLED_R0_INTERRUPT);
        idt[9]   = create_idt_entry((uint32_t) irq_exception_9,   0x8, ENABLED_R0_INTERRUPT);
        idt[10]  = create_idt_entry((uint32_t) irq_exception_10,  0x8, ENABLED_R0_INTERRUPT);
        idt[11]  = create_idt_entry((uint32_t) irq_exception_11,  0x8, ENABLED_R0_INTERRUPT);
        idt[12]  = create_idt_entry((uint32_t) irq_exception_12,  0x8, ENABLED_R0_INTERRUPT);
        idt[13]  = create_idt_entry((uint32_t) irq_exception_13,  0x8, ENABLED_R0_INTERRUPT);
        idt[14]  = create_idt_entry((uint32_t) irq_exception_14,  0x8, ENABLED_R0_INTERRUPT);
        idt[15]  = create_idt_entry((uint32_t) irq_exception_15,  0x8, ENABLED_R0_INTERRUPT);
        idt[16]  = create_idt_entry((uint32_t) irq_exception_16,  0x8, ENABLED_R0_INTERRUPT);
        idt[17]  = create_idt_entry((uint32_t) irq_exception_17,  0x8, ENABLED_R0_INTERRUPT);
        idt[18]  = create_idt_entry((uint32_t) irq_exception_18,  0x8, ENABLED_R0_INTERRUPT);
        idt[19]  = create_idt_entry((uint32_t) irq_exception_19,  0x8, ENABLED_R0_INTERRUPT);
        idt[20]  = create_idt_entry((uint32_t) irq_exception_20,  0x8, ENABLED_R0_INTERRUPT);
        idt[21]  = create_idt_entry((uint32_t) irq_exception_21,  0x8, ENABLED_R0_INTERRUPT);
        idt[22]  = create_idt_entry((uint32_t) irq_exception_22,  0x8, ENABLED_R0_INTERRUPT);
        idt[23]  = create_idt_entry((uint32_t) irq_exception_23,  0x8, ENABLED_R0_INTERRUPT);
        idt[24]  = create_idt_entry((uint32_t) irq_exception_24,  0x8, ENABLED_R0_INTERRUPT);
        idt[25]  = create_idt_entry((uint32_t) irq_exception_25,  0x8, ENABLED_R0_INTERRUPT);
        idt[26]  = create_idt_entry((uint32_t) irq_exception_26,  0x8, ENABLED_R0_INTERRUPT);
        idt[27]  = create_idt_entry((uint32_t) irq_exception_27,  0x8, ENABLED_R0_INTERRUPT);
        idt[28]  = create_idt_entry((uint32_t) irq_exception_28,  0x8, ENABLED_R0_INTERRUPT);
        idt[29]  = create_idt_entry((uint32_t) irq_exception_29,  0x8, ENABLED_R0_INTERRUPT);
        idt[30]  = create_idt_entry((uint32_t) irq_exception_30,  0x8, ENABLED_R0_INTERRUPT);

        idt[0x80] = create_idt_entry((uint32_t) irq_128,  0x8, ENABLED_R3_INTERRUPT);

        // Set PIC masks and PIT
        pic::init(PIC_MASK_PIT_AND_KEYBOARD, PIC_MASK_ALL);
        pit::init();
    }

    void on_interrupt(const uint32_t irq, const cpu::Registers)
    {
        switch (irq)
        {
            case 0: pit::reload(); break;

            case 1:
            {
                const uint8_t scancode = cpu::inb(0x60);

                if (keyboard_buffer_keys < keyboard_buffer_size)
                    keyboard_buffer[keyboard_buffer_keys++] = scancode;
                else
                    println("keyboard buffer is too full to accept scancode ", scancode);

                break;
            }

            default:
                assert(false);
        };

        pic::end_interrupt((uint8_t)irq);
    }

    void on_exception(const uint32_t irq, const cpu::Registers registers)
    {
        static char const* reasons[32] =
        {
            "Divide by zero",               "Debug",
            "Non-maskable interrupt",       "Breakpoint",
            "Overflow",                     "Bound range exceeded",
            "Invalid opcode",               "Device not available",
            "Double fault",                 "Coprocessor segment overrun",
            "Invalid task state segment",   "Segment not present",
            "Stack-segment fault",          "General protection fault",
            "Page fault",                   "Reserved",
            "x87 floating-point exception", "Alignment check",
            "Machine check",                "SIMD floating-point exception",
            "Virtualisation exception",     "Reserved",
            "Reserved",                     "Reserved",
            "Reserved",                     "Reserved",
            "Reserved",                     "Reserved",
            "Reserved",                     "Reserved",
            "Security exception_",
        };

        uart::write_string("\n\n---------- CPU exception ---------- \n");
        println(reasons[irq]);
        println("eax = ", registers.eax);
        println("ebx = ", registers.ebx);
        println("ecx = ", registers.ecx);
        println("edx = ", registers.edx);
        println("esp = ", registers.esp);
        println("ebp = ", registers.ebp);
        println("esi = ", registers.esi);
        println("edi = ", registers.edi);
        if (multitask::current_process != nullptr)
            println("task = ", multitask::current_process->thread_id);
        else
            println("task = kernel!");
        uart::write_string("----------------------------------- \n\n\n");
        halt();
        assert(false);
    }
}