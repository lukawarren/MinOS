section data

message:
    db "Hello world\n"

section text

global _start
_start:
    mov eax, 1
    mov ebx, message
    mov ecx, 11
    int 80

    jmp $
