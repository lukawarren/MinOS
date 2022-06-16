section data

exception_has_no_error_code:
    db 0

section text

global blank_irq
blank_irq:
    iret

; Generic interrupt handler
%macro irq 1
global irq_%1
irq_%1:
    pushad

    cld ; The SysV ABI requires df be clear :-)
    push dword %1
    extern on_interrupt
    call on_interrupt
    add esp, 4

    popad
    iret
%endmacro

; Generic exception handler
%macro exception 2
global exception_%1
exception_%1:

    ; If an error code is present (as in
    ; some exceptions), it needs to be
    ; manually popped off the stack
    cmp byte [exception_has_no_error_code], %2
    je exception_return_%1
    add esp, 4

exception_return_%1:
    iret
%endmacro

irq 0
irq 1
irq 2
irq 3
irq 4
irq 5
irq 6
irq 7
irq 8
irq 9
irq 10
irq 11
irq 12
irq 13
irq 14
irq 15

exception 0,  0
exception 1,  0
exception 2,  0
exception 3,  0
exception 4,  0
exception 5,  0
exception 6,  0
exception 7,  0
exception 8,  1
exception 9,  0
exception 10, 1
exception 11, 1
exception 12, 1
exception 13, 1
exception 14, 1
exception 15, 0
exception 16, 0
exception 17, 1
exception 18, 0
exception 19, 0
exception 20, 0
exception 21, 0
exception 22, 0
exception 23, 0
exception 24, 0
exception 25, 0
exception 26, 0
exception 27, 0
exception 28, 0
exception 29, 0
exception 30, 1