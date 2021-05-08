section text

extern OnInterrupt

; Dummy interrupt handler
global BlankIRQ
BlankIRQ:
    iret

; Generic interrupt handler
%macro IRQHandler 1
global IRQ%1
IRQ%1:
    ; Preserve state - no
    ; need to change  or save
    ; segment descriptors yet,
    ; and iret will take care
    ; of eflags and the stack :-)
    pushad

    xchg bx, bx

    ; Push IRQ and call C++ code
    push %1
    call OnInterrupt
    pop eax

    ; Return
    popad
    iret
%endmacro

; IRQs
IRQHandler 0
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
