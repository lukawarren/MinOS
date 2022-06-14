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

section .text
bits 32

_start:

    ; call main kernel
    extern main
    call main

    ; halt if all else fails
    hlt