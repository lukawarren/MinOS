section data

global userModeAddress
userModeAddress:
    dd 0

section text

extern tss
extern FlushTSSRing3

global SwitchToUserMode
SwitchToUserMode:

    cli             ; Disable interrupts

    ; Update TSS ring 0 esp and stack segment
    push eax
    mov eax, [tss]
    add esp, 4
    mov [eax+4], esp
    mov [eax+8], dword 0x10
    sub esp, 4
    pop eax

    ; Setup segments with ring 3 data selector
    mov ax, 0x23        ; ring 3 data selector with bottom 2 bits set for ring 3
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax          ; ss is handled by iret

    ; Setup the stack frame for an "interrupt" 
    ; which returns to user mode
    mov eax, esp
    push 0x23           ; ring 3 data selector with bottom 2 bits set for ring 3
    push eax            ; preserved user mode esp

    ; Modify pushed eflags to enable interrupts again,
    ; as "sti" won't work in user mode
    pushf               ; user mode eflags
    pop eax
    or eax, 0x200
    push eax

    push 0x1B               ; ring 3 code selector with bottom 2 bits set for ring 3
    push dword [userModeAddress] ; eip to return to

    ; Now simply "return" from the "interrupt"
    iret
