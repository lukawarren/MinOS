global _start

MBALIGN  equ  1 << 0            ; align loaded modules on page boundaries
MEMINFO  equ  1 << 1            ; provide memory map
GFXINFO  equ  1 << 2			; provide video mode table
FLAGS    equ  MBALIGN | MEMINFO | GFXINFO ; this is the Multiboot 'flag' field
MAGIC    equ  0x1BADB002        ; 'magic number' lets bootloader find the header
CHECKSUM equ -(MAGIC + FLAGS)   ; checksum of above, to prove we are multiboot

; Multiboot
section .multiboot
align 4
	dd MAGIC
	dd FLAGS
	dd CHECKSUM
	dd 0, 0, 0, 0, 0
	dd 0  ; set graphics mode
	dd 640, 480, 32 ; width, height, depth

; Stack
section .bss
align 16
stack_bottom:
    resb 65536 ; 64 KiB
stack_top:

section .text
bits 32

_start:

    ; set up stack
    mov esp, stack_top

    ; call main kernel (modifying esp
    ; for 16 byte stack alignment)
    sub esp, 12
    push eax ; multiboot eax magic
    push ebx ; multiboot header address
    extern kmain
    call kmain
    pop ebx
    pop eax
    add esp, 12

    ; halt if all else fails
    hlt