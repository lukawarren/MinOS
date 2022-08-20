section data

return_eax_storage: dd 0
cr3_storage: dd 0

section text
extern on_syscall

global irq_128
irq_128:

    ; Run handler (return code in eax)
    pushad
    cld ; The SysV ABI requires df be clear :-)
    call on_syscall
    mov [return_eax_storage], eax
    popad

    ; Clobber for return value
    mov eax, [return_eax_storage]

    iret