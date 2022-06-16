pub mod interrupts;
mod pic;

extern "C"
{
    fn blank_irq();

    fn irq_0();
    fn irq_1();
    fn irq_2();
    fn irq_3();
    fn irq_4();
    fn irq_5();
    fn irq_6();
    fn irq_7();
    fn irq_8();
    fn irq_9();
    fn irq_10();
    fn irq_11();
    fn irq_12();
    fn irq_13();
    fn irq_14();
    fn irq_15();

    fn exception_0();
    fn exception_1();
    fn exception_2();
    fn exception_3();
    fn exception_4();
    fn exception_5();
    fn exception_6();
    fn exception_7();
    fn exception_8();
    fn exception_9();
    fn exception_10();
    fn exception_11();
    fn exception_12();
    fn exception_13();
    fn exception_14();
    fn exception_15();
    fn exception_16();
    fn exception_17();
    fn exception_18();
    fn exception_19();
    fn exception_20();
    fn exception_21();
    fn exception_22();
    fn exception_23();
    fn exception_24();
    fn exception_25();
    fn exception_26();
    fn exception_27();
    fn exception_28();
    fn exception_29();
    fn exception_30();
}