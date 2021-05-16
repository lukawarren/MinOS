section data

extern kernelCR3

; For switching tasks after _exit
global bSwitchTasks
bSwitchTasks:
    db 0

section text

extern OnSyscall
extern SwitchToNewTask

; Syscall interrupt handler
global IRQ80
IRQ80:
    ; Preserve registers
    pushad
    mov eax, cr3
    push eax
    mov eax, [kernelCR3]
    mov cr3, eax

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

    ; Call C++ code
    call OnSyscall

    ; Whatever was returned (if anything)
    ; now lies in eax, but C++ set it
    ; directly so there's no need
    ; to treat anything differently

    ; Restore segment registers
    pop gs
    pop fs
    pop es
    pop ds

    ; Check if we need to switch tasks
    cmp byte [bSwitchTasks], 1
    jne finishSyscall
    mov byte [bSwitchTasks], 0

    ; If we are switching, mend stack first
    ; (we're not restoring the old cr3 you see)
    add esp, 4
    popad

    ; Our iret will use the new task's stack,
    ; so we need to remove the current iret frame,
    ; which is 6 dword pops, or 4*6 = 24
    add esp, 24

    jmp SwitchToNewTask

finishSyscall:
    ; Restore registers
    pop eax
    mov cr3, eax
    popad

    ; Return
    iret