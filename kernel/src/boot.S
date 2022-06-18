global _start

section .multiboot
header_start:
    dd 0xe85250d6                                                   ; magic number (multiboot 2)
    dd 0                                                            ; architecture 0 (protected mode i386)
    dd header_end - header_start                                    ; header length
    dd 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start)) ; checksum

    ; end tag
    dw 0    ; type
    dw 0    ; flags
    dd 8    ; size
header_end:

; Stack
section .bss
align 16
stack_bottom:
    resb 16384 ; 16 KiB
stack_top:

section .text
bits 32

_start:

    ; set up stack
    mov esp, stack_top

    ; call main kernel (adding to esp
    ; for stack alignment)
    sub esp, 12
    push ebx
    extern main
    call main
    pop ebx
    add esp, 12

    ; halt if all else fails
    hlt