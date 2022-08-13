#pragma once
#include "cpu/cpu.h"

namespace interrupts
{
    extern "C"
    {
        void on_interrupt(const uint32_t irq, const cpu::Registers registers);
        void on_exception(const uint32_t irq, const cpu::Registers registers);

        extern void blank_irq();

        extern void irq_0();
        extern void irq_1();
        extern void irq_2();
        extern void irq_3();
        extern void irq_4();
        extern void irq_5();
        extern void irq_6();
        extern void irq_7();
        extern void irq_8();
        extern void irq_9();
        extern void irq_10();
        extern void irq_11();
        extern void irq_12();
        extern void irq_13();
        extern void irq_14();
        extern void irq_15();

        extern void irq_exception_0();
        extern void irq_exception_1();
        extern void irq_exception_2();
        extern void irq_exception_3();
        extern void irq_exception_4();
        extern void irq_exception_5();
        extern void irq_exception_6();
        extern void irq_exception_7();
        extern void irq_exception_8();
        extern void irq_exception_9();
        extern void irq_exception_10();
        extern void irq_exception_11();
        extern void irq_exception_12();
        extern void irq_exception_13();
        extern void irq_exception_14();
        extern void irq_exception_15();
        extern void irq_exception_16();
        extern void irq_exception_17();
        extern void irq_exception_18();
        extern void irq_exception_19();
        extern void irq_exception_20();
        extern void irq_exception_21();
        extern void irq_exception_22();
        extern void irq_exception_23();
        extern void irq_exception_24();
        extern void irq_exception_25();
        extern void irq_exception_26();
        extern void irq_exception_27();
        extern void irq_exception_28();
        extern void irq_exception_29();
        extern void irq_exception_30();
    }
}