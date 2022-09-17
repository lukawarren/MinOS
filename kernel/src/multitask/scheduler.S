section data

; ----------------- set from C++ -----------------
global kernel_cr3
kernel_cr3: dd 0

global old_stack_address
old_stack_address: dd 0

global old_fxsave_address
align 16
old_fxsave_address: dd 0

global new_stack_address
new_stack_address: dd 0

global new_fxsave_address
align 16
new_fxsave_address: dd 0

; ----------------- set from C++ -----------------

; When switching rings, the kernel esp is loaded instead of the current task's.
; As we intend on switching stacks to a new task and iret'ing there, this
; will mean that unless we manually pop the iret frame off the kernel every time,
; it'll overflow!
iret_stack:
    dd 0
    dd 0
    dd 0
    dd 0
    dd 0
    dd 0

; With all our stack switching, it's less mental effort to store stuff
; in memory rather than juggling stacks
old_cr3: dd 0


section text
extern on_scheduler

global irq_0
irq_0:
    ; extract iret stack
    pop dword [iret_stack + 0]
    pop dword [iret_stack + 4]
    pop dword [iret_stack + 8]
    pop dword [iret_stack + 12]
    pop dword [iret_stack + 16]
    pop dword [iret_stack + 20]

    ; save old FPU before we stich task frames and lose access to it... if the
    ; address is non-0 (nullptr is used in C++ to indicate a "don't save")
    ;
    ; NOTE: we actually use the *new* fxsave address because we've set this
    ;       variable in the C++ code *the previvous iteration*, meaning
    ;       that although it's old now, it was actually new then!
    pushad
    cmp [new_fxsave_address], dword 0
    je switch_frames
    mov eax, [new_fxsave_address]
    fxsave [eax]

switch_frames:
    popad

    ; use kernel cr3, save old one
    push eax
    mov eax, cr3
    mov [old_cr3], eax
    mov eax, [kernel_cr3]
    mov cr3, eax
    pop eax

    ; call our trusty scheduler, clearing df as per the SysV ABI (popad later
    ; so we don't clobber eflags with our cmp)
    pushad
    cld
    call on_scheduler

    ; a nullptr means we've come from the kernel - we don't care about
    ; preserving registers, so clobbering eflags should be fine
    mov eax, [old_stack_address]
    cmp eax, 0
    je load_new_task
    popad

save_old_task:
    ; Get the userspace stack from the iret frame (as our current esp is the
    ; kernel one set in the TSS)
    mov esp, [iret_stack + 12]

    ; save iret frame
    push dword [iret_stack + 20]
    push dword [iret_stack + 16]
    push dword [iret_stack + 12]
    push dword [iret_stack + 8]
    push dword [iret_stack + 4]
    push dword [iret_stack + 0]

    ; save general purpose registers
    pushad

    ; save segment registers
    mov eax, ds
    push eax
    mov eax, fs
    push eax
    mov eax, es
    push eax
    mov eax, gs
    push eax

    ; save cr3
    mov eax, [old_cr3]
    push eax

    ; finally, save the task's esp (we've taken a pointer to it)
    mov eax, [old_stack_address]
    mov [eax], esp

load_new_task:

    ; switch to new task stack
    mov esp, [new_stack_address]
    mov esp, [esp]

    ; restore cr3
    pop eax
    mov cr3, eax

    ; restore FPU state
    mov eax, [new_fxsave_address]
    fxrstor [eax]

    ; restore segment registers
    pop eax
    mov gs, ax
    pop eax
    mov es, ax
    pop eax
    mov fs, ax
    pop eax
    mov ds, ax

    ; restore general registers
    popad

    ; restore iret frame - success!
    iret
