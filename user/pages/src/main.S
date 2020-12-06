section text

global main
main:

    ; testing value
    mov ecx, 0xdeadbeef

    ; delay
    mov eax, 0xbeef
delay:
    cmp eax, 0
    je syscall
    sub eax, 1
    jmp delay

syscall:
    pusha
    mov eax, 0
    int 0x80
    popa
    jmp main