section data

; Set by Rust paging code
extern KERNEL_CR3

; When switching rings, the kernel
; esp is loaded instead of our own.
; As we intend on switching stacks
; to a new task and iret'ing there,
; this will mean that unless we
; manually pop the iret frame off
; the kernel every time, it'll overflow!
iret_stack:
    dd 0
    dd 0
    dd 0
    dd 0
    dd 0
    dd 0

; It is easier to store stuff
; in memory rather than mess
; around with stacks
old_cr3: dd 0
assembly_info_address: dd 0

section text

extern change_tasks

global on_pit_interrupt
on_pit_interrupt:

    ; Extract iret stack
    pop dword [iret_stack + 0]
    pop dword [iret_stack + 4]
    pop dword [iret_stack + 8]
    pop dword [iret_stack + 12]
    pop dword [iret_stack + 16]
    pop dword [iret_stack + 20]

    ; Switch to the kernel cr3
    ; so that different tasks'
    ; stacks are mapped in
    push eax
    mov eax, cr3
    mov [old_cr3], eax
    mov eax, [KERNEL_CR3],
    mov cr3, eax
    pop eax

    ; Call Rust code
    pushad
    push dword 1
    cld ; df needs to be clear as per the SysV ABI
    call change_tasks ; returns value into eax
    mov [assembly_info_address], eax
    add esp, 4
    popad

    ; Load assembly_info_address and
    ; use it to get "came_from_kernel",
    ; using esp because we can clobber
    ; that register
    mov esp, [assembly_info_address]
    mov esp, [esp + 8]

    ; Don't save if we came from kernel
    cmp esp, 1
    je load_new_task

preserve_old_task:

    ; Retrieve original caller esp from the
    ; iret frame (the one being used right
    ; now should be the kernel one from the TSS)
    mov esp, [iret_stack + 12]

    ; iret frame
    push dword [iret_stack + 20]
    push dword [iret_stack + 16]
    push dword [iret_stack + 12]
    push dword [iret_stack + 8]
    push dword [iret_stack + 4]
    push dword [iret_stack + 0]

    ; General registers
    pushad

    ; Segment registers (32-bit for easier Rust code)
    mov eax, ds
    push eax
    mov eax, fs
    push eax
    mov eax, es
    push eax
    mov eax, gs
    push eax

    ; CR3
    mov eax, [old_cr3]
    push eax

    ; Save "old_stack_address" (which is a
    ; pointer to the esp, not the esp itself,
    ; so that it won't break if the old task
    ; and new task are the same!)
    mov eax, [assembly_info_address]
    mov eax, [eax + 4]
    mov [eax], esp

extern load_new_task ; used by syscalls
load_new_task:

    ; Load "new_stack_address"
    mov esp, [assembly_info_address]
    mov esp, [esp]
    mov esp, [esp]

    ; CR3
    pop eax
    mov cr3, eax

    ; Segment registers (see above)
    pop eax
    mov gs, ax
    pop eax
    mov es, ax
    pop eax
    mov fs, ax
    pop eax
    mov ds, ax

    ; General registers
    popad

    ; Restore the iret frame
    iret
