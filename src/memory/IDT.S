section text

global LoadIDT

LoadIDT:
    mov edx, [esp + 4]
    lidt [edx]
    sti ; enable interrupts
    ret

extern HandleInterrupts

%macro IRQHandler 1
global IRQ%1
IRQ%1:
    push    eax                 ; Preserve state
    push    dword 0             ; This is not an error
    push    dword %1            ; Push interrupt id
    call    HandleInterrupts    ; Call generic function
    pop     eax                 ; Restore state
    pop     eax
    pop     eax
    iret                        ; Return
%endmacro

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

global IRQUnknown
IRQUnknown:
    push    eax                 ; Preserve state
    push    dword 1             ; This is an error!
    push    dword 0xAF          ; Push superfluous id
    call    HandleInterrupts    ; Call generic function
    pop     eax                 ; Restore state
    pop     eax
    pop     eax
    iret                        ; Return