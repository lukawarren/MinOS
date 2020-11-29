section data

global IRQReturnAddress
IRQReturnAddress:
    dd 0

global bIRQShouldJump
bIRQShouldJump:
    db 0

global IRQJumpEIP
IRQJumpEIP:
    dd 0

section text

global LoadIDT

LoadIDT:
    cli ; disable interrupts
    push eax
    mov eax, [esp + 8]
    lidt [eax]
    pop eax
    sti ; enable interrupts
    ret

extern HandleInterrupts

%macro IRQHandler 1
global IRQ%1
IRQ%1:
    cli ; Disable interrupts until iret is called

    ; Preserve state
    push eax
    push ebx
    push ecx
    push edx
    push ebp
    push edi
    push esi
    push esp

    push    dword 0             ; This is not an error
    push    dword %1            ; Push interrupt id
    call    HandleInterrupts    ; Call generic function
    pop     eax
    pop     eax

    ; Restore state
    pop esp
    pop esi
    pop edi
    pop ebp
    pop edx
    pop ecx
    pop ebx
    pop eax

    ; Perform IRQ jump if nessecaryy
    cmp byte [bIRQShouldJump], 1
    je  PerformIRQJump%1
    jmp FinishJump%1

PerformIRQJump%1:
    push eax

    ; Set IRQJumpEIP as jump is not following calling conventions otherwises
    mov eax, [esp+4]
    mov [IRQJumpEIP], eax

    mov eax, [IRQReturnAddress]
    mov [esp+4], eax ; Set the return address to the required process
    pop eax

    mov [bIRQShouldJump], byte 0 ; Set bIRQShouldJump to false
    
FinishJump%1:
    iret    ; Return
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