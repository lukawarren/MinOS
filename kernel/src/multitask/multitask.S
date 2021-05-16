section data

; Upon a context switch, we save
; the old registers to the
; stack of the process in question.
; We get this stack from the iret
; frame, but afterwards we need
; to update the stack so that it
; can be restored later
global pSavedTaskStack
pSavedTaskStack:
    dd 0

; We then switch to the new task
; and restore its registers -
; this is a pointer too incase
; we're switching back to the same
; task, as we want to use the
; newly-soiled stack pointer
global pNewTaskStack
pNewTaskStack:
    dd 0

; We can't manipulate another
; task's stack if we're using
; a user task's cr3 register,
; so we need a global, kernel
; one
extern kernelCR3

; If the privilege (ring) changes,
; the old esp is pushed onto the
; stack, else the TSS stack is
; not used, and the old stack
; is just the current stack
; minus all the iret stuff.
; This makes knowing what our
; esp was a bit of a pain!
global bPrivilegeChange
bPrivilegeChange:
    db 0

oldStack:
    dd 0

; When switching rings, our stack
; pointer is preserved, and
; we end up using the kernel one
; instead. If we're not careful
; this will mess up our stack frame
; since the kernel stack will keep
; getting pushed onto, and the old
; task, despite having been interrupted,
; won't have had its iret frame in place.
; To this end, the code "shifts" the iret
; frame over to the correct stack
iretStack:
    dd 0
    dd 0
    dd 0
    dd 0
    dd 0
    dd 0

; We also need to preserve cr3
oldCR3:
    dd 0

section text

extern OnPIT

; PIT interrupt handler
global IRQ0
IRQ0:
    ; First things first, "extract"
    ; iret stack (see above)
    pop dword [iretStack+0]
    pop dword [iretStack+4]
    pop dword [iretStack+8]
    pop dword [iretStack+12]
    pop dword [iretStack+16]
    pop dword [iretStack+20]

    ; We need to use the kernel's cr3
    ; so we can fully modify everyone's
    ; stacks without fear of a page fault!
    push eax
    mov eax, cr3
    mov [oldCR3], eax
    mov eax, [kernelCR3]
    mov cr3, eax
    pop eax

    ; If no privilege transition occured,
    ; we're in luck, as we can just use
    ; the stack present here :)
    cmp byte [bPrivilegeChange], 0
    jne getStackFromIRET
    mov [oldStack], esp
    je callCPPCode

getStackFromIRET:
    ; Otherwise, get it from iret
    push eax
    mov eax, [iretStack+12]
    mov [oldStack], eax
    pop eax

callCPPCode:
    ; Preserve registers
    pushad

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
    call OnPIT

    ; Restore segment registers
    pop gs
    pop fs
    pop es
    pop ds

    ; Restore registers
    popad

    ; Now we've called the C++ code,
    ; we know what task to switch to.
    ; Preserve the old, then in with
    ; the new! However if we're coming
    ; directly from the kernel, we
    ; don't care what we trash, so
    ; anything goes!

    cmp dword [pSavedTaskStack], 0
    je SwitchToNewTask

preserveOldTask:
    ; Move to old task's stack (which
    ; we may or may not be in, depending
    ; on if we changed rings!)
    mov esp, [oldStack]

    ; Iret stack frame
    push dword [iretStack+20]
    push dword [iretStack+16]
    push dword [iretStack+12]
    push dword [iretStack+8]
    push dword [iretStack+4]
    push dword [iretStack+0]
    
    ; Registers
    pushad

    ; Segment registers - 32 bit
    ; for easier C++ code
    mov eax, ds
    push eax
    mov eax, fs
    push eax
    mov eax, es
    push eax
    mov eax, gs
    push eax

    ; CR3
    mov eax, [oldCR3]
    push eax

    ; Update the stack pointer,
    ; which we have a pointer to
    mov eax, [pSavedTaskStack]
    mov [eax], esp

    jmp SwitchToNewTask

; Global because it's called from
; the _exit syscall
global SwitchToNewTask
SwitchToNewTask:
    ; Switch to new stack
    mov eax, [pNewTaskStack]
    mov esp, [eax]

    ; Restore cr3
    pop eax
    mov cr3, eax

    ; Restore segment registers
    pop eax
    mov gs, ax
    pop eax
    mov es, ax
    pop eax
    mov fs, ax
    pop eax
    mov ds, ax

    ; Restore general purpose registers
    popad

    ; Return and let iret restore
    ; everything else
    iret
