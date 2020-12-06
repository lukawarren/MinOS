section text

global main
main:
    ; testing value
    mov ecx, 0xdeadbeef

    ; delay

    mov eax, 100000000
delay:
    cmp eax, 0
    je syscall
    sub eax, 1
    jmp delay

syscall:
    mov eax, 0

    push eax
    push ebx
    push ecx
    push edx
    push ebp
    push edi
    push esi
    int 0x80
    pop esi
    pop edi
    pop ebp
    pop edx
    pop ecx
    pop ebx
    pop eax
    jmp main