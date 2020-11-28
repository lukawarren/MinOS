section text

global SwitchToTask

SwitchToTask:

    ; Disable IRQs
    cli

    ; Save current task - eip is already saved by the caller's "call" instruction
    push eax
    push ebx
    push ecx
    push edx
    push edi
    push esi
    push ebp

    ; Get address of new stack
    mov esi, [esp+4+7*4] ; contains address of new stack

    ; Switch to new task
    mov esp, esi
    
    pop ebp
    pop esi
    pop edi
    pop edx
    pop ecx
    pop ebx
    pop eax

    ; Enable IRQs
    sti

    ret