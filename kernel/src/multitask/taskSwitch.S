section data

extern IRQJumpEIP

global oldTaskStack
oldTaskStack:
    dd 0

global newTaskStack
newTaskStack:
    dd 0

iretStack:
    dd 0
    dd 0
    dd 0
    dd 0
    dd 0
    dd 0

section text

global PerformTaskSwitch
PerformTaskSwitch:

    ; Move iret stack off kernel stack
    pop dword [iretStack+0]
    pop dword [iretStack+4]
    pop dword [iretStack+8]
    pop dword [iretStack+12]
    pop dword [iretStack+16]
    pop dword [iretStack+20]

    ; If there was a previous task
    ; (not just the kernel main thread),
    ; save its stack
    cmp [oldTaskStack], dword 0
    je switchToNewTask

    ; Switch to old stack before interrupt
    ; occured by getting it off the iret
    ; stack frame
    mov esp, [iretStack+12]

    ; Construct iret
    push dword [iretStack+20]
    push dword [iretStack+16]
    push dword [iretStack+12]
    push dword [iretStack+8]
    push dword [iretStack+4]
    push dword [iretStack+0]

    ; Push old state onto its stack
    push eax
    push ebx
    push ecx
    push edx
    push edi
    push esi
    push ebp

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

    ; Save old task's stack
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
    pop ebp
    pop esi
    pop edi
    pop edx
    pop ecx
    pop ebx
    pop eax

    iret ; Return anew

global PerformOneWaySwitch
PerformOneWaySwitch:
    jmp switchToNewTask