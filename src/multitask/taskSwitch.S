section data

extern IRQJumpEIP

global oldTaskStack
oldTaskStack:
    dd 0

global newTaskStack
newTaskStack:
    dd 0

section text

global PerformTaskSwitch

PerformTaskSwitch:

    ; Disable IRQs
    cli

    push dword [IRQJumpEIP] ; eip is not already saved as there was no "call" instruction
    push eax
    push ebx
    push ecx
    push edx
    push edi
    push esi
    push ebp

    ; Save esp of previous task
    ; (if indeed there was one)
    cmp [oldTaskStack], dword 0
    je switchToNewTask
saveOldStack:
    mov edx, [oldTaskStack] ; contains pointer to old stack
    mov [edx], esp

switchToNewTask:
    ; Switch to new task
    mov edx, [newTaskStack]
    mov esp, [edx] ; contains address of new stack
    
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