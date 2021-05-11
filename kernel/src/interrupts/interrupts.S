section data

; Used in exceptions to
; avoid trashing registers
nonErrorCodeValue:
    db 0

section text

extern OnInterrupt
extern OnException

; Dummy interrupt handler
global BlankIRQ
BlankIRQ:
    iret

; Generic interrupt handler
%macro IRQHandler 1
global IRQ%1
IRQ%1:

    ; Preserve registers
    pushad

    ; Preserve segment registers
    ; (in case we were called from
    ; user mode)
    push ds
    push es
    push fs
    push gs

    ; Use ring 0 segment registers
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Push IRQ and call C++ code
    push dword %1
    call OnInterrupt
    pop eax

    ; Restore segment registers
    pop gs
    pop fs
    pop es
    pop ds

    ; Restore registers
    popad

    ; Return
    iret
%endmacro

; Generic exception handler
%macro ExceptionHandler 2
global IRQException%1
IRQException%1:

    ; Preserve registers
    pushad

    ; Preserve segment registers
    ; (in case we were called from
    ; user mode)
    push ds
    push es
    push fs
    push gs

    ; Use ring 0 segment registers
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push dword %1
    call OnException
    pop eax

    ; Restore segment registers
    pop gs
    pop fs
    pop es
    pop ds

    ; Restore registers
    popad

    ; Exceptions with an error code
    ; need to be manually pulled off
    ; the stack
    cmp byte [nonErrorCodeValue], %2
    je returnFromException%1
    add esp, 4

returnFromException%1:
    iret
%endmacro

; IRQs - 0 (PIT) is handled separately
IRQHandler 1
IRQHandler 2
IRQHandler 3
IRQHandler 4
IRQHandler 5
IRQHandler 6
IRQHandler 7
IRQHandler 8
IRQHandler 9
IRQHandler 10
IRQHandler 11
IRQHandler 12
IRQHandler 13
IRQHandler 14
IRQHandler 15

; Exceptions - 2nd number is 1 if an
; error code is present, 0 if not
ExceptionHandler 0,  0
ExceptionHandler 1,  0
ExceptionHandler 2,  0
ExceptionHandler 3,  0
ExceptionHandler 4,  0
ExceptionHandler 5,  0
ExceptionHandler 6,  0
ExceptionHandler 7,  0
ExceptionHandler 8,  1
ExceptionHandler 9,  0
ExceptionHandler 10, 1
ExceptionHandler 11, 1
ExceptionHandler 12, 1
ExceptionHandler 13, 1
ExceptionHandler 14, 1
ExceptionHandler 15, 0
ExceptionHandler 16, 0
ExceptionHandler 17, 1
ExceptionHandler 18, 0
ExceptionHandler 19, 0
ExceptionHandler 20, 0
ExceptionHandler 21, 0
ExceptionHandler 22, 0
ExceptionHandler 23, 0
ExceptionHandler 24, 0
ExceptionHandler 25, 0
ExceptionHandler 26, 0
ExceptionHandler 27, 0
ExceptionHandler 28, 0
ExceptionHandler 29, 0
ExceptionHandler 30, 1
