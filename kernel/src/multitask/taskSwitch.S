section data

extern IRQJumpEIP

global oldTaskStack
oldTaskStack:
    dd 0

global newTaskStack
newTaskStack:
    dd 0

bob:
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

    ; Segment registers
    ; 32-bits for easier C code
    mov eax, ds
    push eax
    mov eax, fs
    push eax
    mov eax, es
    push eax
    mov eax, gs
    push eax

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

    ; Segment registers
    ; 32-bits for easier C code
    pop eax
    mov gs, ax
    pop eax
    mov es, ax
    pop eax
    mov fs, ax
    pop eax
    mov ds, ax

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

global PerformOneWaySwitch
PerformOneWaySwitch:
    jmp switchToNewTask