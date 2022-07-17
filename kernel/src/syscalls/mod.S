section data

extern KERNEL_CR3
extern interrupt_cr3_holder

extern syscall_return
syscall_return:
    dd 0    ; return value
    db 0    ; if task exited (bool)

syscall_return_value: dd 0

section text

extern on_syscall
extern load_new_task

global irq_80
irq_80:
    ; Save cr3
    push eax
    mov eax, cr3
    mov [interrupt_cr3_holder], eax

    ; Load kernel cr3
    mov eax, [KERNEL_CR3]
    mov cr3, eax
    pop eax

    ; Run syscall
    pushad
    cld ; The SysV ABI requires df be clear :-)
    call on_syscall
    popad

    ; Check if task exited
    mov eax, [syscall_return+4]
    cmp eax, 1
    je load_new_task

return_to_old_task:
    ; Set return value
    mov eax, [syscall_return]

    ; Restore cr3
    push eax
    mov eax, [interrupt_cr3_holder]
    mov cr3, eax
    pop eax

    iret
