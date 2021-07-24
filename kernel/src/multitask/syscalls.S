section data

extern kernelCR3
extern bSwitchTasks

eaxBuffer:
    dd 0

global bSaveTaskBeforeSwitching
bSaveTaskBeforeSwitching:
    db 0

; For block()
iretStack:
    dd 0
    dd 0
    dd 0
    dd 0
    dd 0
    dd 0

; For block()
taskCR3:
    dd 0

section text

extern OnSyscall
extern SwitchToNewTask
extern SaveOldStackESP

; Syscall interrupt handler
global IRQ80
IRQ80:
    ; For block(), copy iret frame and CR3
    pop dword [iretStack+0]
    pop dword [iretStack+4]
    pop dword [iretStack+8]
    pop dword [iretStack+12]
    pop dword [iretStack+16]
    pop dword [iretStack+20]
    push eax
    mov eax, cr3
    mov [taskCR3], eax
    pop eax

    ; ...then push it back on!
    ; (well, y'know I mean computers
    ; are so fast these days, who
    ; *really* cares?)
    push dword[iretStack+20]
    push dword[iretStack+16]
    push dword[iretStack+12]
    push dword[iretStack+8]
    push dword[iretStack+4]
    push dword[iretStack+0]

    ; Preserve registers
    pushad

    ; Preserve CR3
    mov eax, cr3
    push eax
    mov eax, [kernelCR3]
    mov cr3, eax

    ; Preserve segment registers
    ; (in case we were called from
    ; user mode)
    push ds
    push es
    push fs
    push gs

    ; Use ring 0 segment registers
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Call C++ code
    call OnSyscall

    ; Whatever was returned (if anything)
    ; now lies in eax, but C++ set it
    ; directly so there's no need
    ; to treat anything differently
    mov [eaxBuffer], eax

    ; Restore segment registers
    pop gs
    pop fs
    pop es
    pop ds

    ; Check if we need to switch tasks
    cmp byte [bSwitchTasks], 1
    jne finishSyscall
    mov byte [bSwitchTasks], 0
    
    ; If we are switching, remove current
    ; stack frame, including CR3 (4 bytes),
    ; pushad (32 bytes), and iret (24 bytes)
    add esp, 60

    ; block() needs the old task saving first
    cmp byte [bSaveTaskBeforeSwitching], 1
    jne newTaskSwitch
    
    ; Before we save stacks then saunter off,
    ; save the state of the task as we would in
    ; the multitasking code (including having the
    ; right stack!)
    mov esp, [esp-12] ; from iret stack
    
    ; 1) iret frame
    push dword[iretStack+20]
    push dword[iretStack+16]
    push dword[iretStack+12]
    push dword[iretStack+8]
    push dword[iretStack+4]
    push dword[iretStack+0]
    
    ; 2) Registers
    pushad
    
    ; 3) Segment registers
    mov eax, ds
    push eax
    mov eax, fs
    push eax
    mov eax, es
    push eax
    mov eax, gs
    push eax
    
    ; 4) CR3
    mov eax, [taskCR3]
    push eax
    
    jmp SaveOldStackESP ; Calls SwitchToNewTask afterwards

newTaskSwitch:
    jmp SwitchToNewTask

finishSyscall:
    ; Restore registers
    pop eax
    mov cr3, eax
    popad

    ; Get back eax
    mov eax, [eaxBuffer]

    ; Return
    iret
