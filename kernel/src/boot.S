; Declare constants for the multiboot header.
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
 
; Stack
section .bss
alig 16
stack_bottom:
resb 16384 ; 16 KiB
stack_top:
 

section .text
global _start:function (_start.end - _start)
_start:

	; Stack
	mov esp, stack_top
	
	; Call C code (double push is for 16-byte stack alignment)
	push ebx
	push ebx
	extern kernel_main
	;call kernel_main
	pop ebx
	pop ebx

	; Hang
	cli
.hang:	hlt
	jmp .hang
.end: