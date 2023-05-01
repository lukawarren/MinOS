section data

extern kernel_cr3
return_eax_storage: dd 0
cr3_storage: dd 0

section text
extern on_syscall

global irq_128
irq_128:
    ; Switch to kernel cr3
    push eax
    mov eax, cr3
    mov [cr3_storage], eax
    mov eax, [kernel_cr3]
    mov cr3, eax
    pop eax

    ; Run handler (return code in eax)
    pushad
    cld ; The SysV ABI requires df be clear :-)
    call on_syscall
    mov [return_eax_storage], eax
    popad

    ; Restore cr3 and clobber eax for return value
    mov eax, [cr3_storage]
    mov cr3, eax
    mov eax, [return_eax_storage]

    iret