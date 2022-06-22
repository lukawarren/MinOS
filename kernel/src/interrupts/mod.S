section data

; Set by Rust paging code
extern KERNEL_CR3

exception_has_no_error_code: db 0
interrupt_cr3_holder: dd 0

section text

extern on_interrupt
extern on_exception

global blank_irq
blank_irq:
    iret


; Generic interrupt handler
%macro irq 1
global irq_%1
irq_%1:

    ; Save cr3
    push eax
    mov eax, cr3
    mov [interrupt_cr3_holder], eax

    ; Load kernel cr3
    mov eax, [KERNEL_CR3]
    mov cr3, eax
    pop eax

    ; Run handler
    pushad
    cld ; The SysV ABI requires df be clear :-)
    push dword %1
    call on_interrupt
    add esp, 4
    popad

    ; Restore cr3
    push eax
    mov eax, [interrupt_cr3_holder]
    mov cr3, eax
    pop eax

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
    je exception_handle_%1
    add esp, 4

exception_handle_%1:
    ; Push error code and registers, call
    ; Rust, then half for now
    pushad
    cld ; see above
    push %1
    call on_exception
    hlt

    iret
%endmacro


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
irq 80

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