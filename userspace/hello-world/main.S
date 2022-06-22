section data

message:
    db "Hello world"

section text

global _start
_start:
    mov eax, 0
    mov ebx, message
    mov ecx, 11
    int 80

    jmp $
