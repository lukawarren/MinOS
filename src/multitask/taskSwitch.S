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

    ; Save esp of previous task
    mov edx, [esp+4+7*4] ; contains pointer to old stack in process struct
    mov [edx], esp

    ; Switch to new task
    mov esp, [esp+4+8*4] ; contains address of new stack
    
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