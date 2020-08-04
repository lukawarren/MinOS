section text

global inb

inb:
    push ebp ; push ebp onto stack
    mov ebp, esp ; move stack pointer to ebp
    mov dx, [ebp + 8] ; use stack pointer to move argument to dx
    in eax, dx ; in with argument into eax
    pop ebp ; restore ebp
    ret

global outb

outb:
    push ebp ; push ebp onto stack
    mov ebp, esp ; move stack pointer to ebp
    mov dx, [ebp + 8] ; port to dx
    mov al, [ebp + 12] ; data to al
    out dx, al ; out al into address dx
    pop ebp ; restore ebp
    ret