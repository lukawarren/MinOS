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

    ; Push old state onto its stack
    push eax
    push ebx
    push ecx
    push edx
    push edi
    push esi
    push ebp
    pushf

    ; If there was a previous task
    ; (not just the kernel main thread),
    ; save its stack
    cmp [oldTaskStack], dword 0
    je switchToNewTask

saveOldStack:

    mov edx, [oldTaskStack] ; contains pointer to old stack
    mov [edx], esp

switchToNewTask:

    ; Switch to new task's stack
    mov edx, [newTaskStack]
    mov esp, [edx] ; contains address of new stack

    ; Restore new task's stack
    popf
    pop ebp
    pop esi
    pop edi
    pop edx
    pop ecx
    pop ebx
    pop eax

    ret ; Return anew