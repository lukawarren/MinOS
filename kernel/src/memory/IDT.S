section data

; Set in multitask.cpp when
; a task swtich is to occur
global bIRQShouldJump
bIRQShouldJump:
    db 0

section text

global LoadIDT

LoadIDT:
    cli ; disable interrupts
    push eax
    mov eax, [esp + 8]
    lidt [eax]
    pop eax
    sti ; enable interrupts
    ret

extern HandleInterrupts
extern HandleExceptions
extern PerformTaskSwitch
extern PerformOneWaySwitch

%macro IRQHandler 1
global IRQ%1
IRQ%1:
    cli ; Disable interrupts until iret is called
    cli ; For some reason bochs does not trigger a
    cli ; breakpoint unless I do 3 in a row, and I am paranoid

    ; Preserve state
    push eax
    push ebx
    push ecx
    push edx
    push ebp
    push edi
    push esi

    ; Preserve segment registers
    ; and use ring 0 ones
    push ds
    push es
    push fs
    push gs

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push    dword 0             ; This is not an error
    push    dword %1            ; Push interrupt id
    call    HandleInterrupts    ; Call generic function
    pop     eax
    pop     eax

    ; Get back segment registers
    pop gs
    pop fs
    pop es
    pop ds

    ; Restore state
    pop esi
    pop edi
    pop ebp
    pop edx
    pop ecx
    pop ebx
    pop eax

    ; Perform IRQ jump if nessecaryy
    cmp [bIRQShouldJump], byte 1
    je  PerformIRQJump%1
    jmp FinishJump%1

PerformIRQJump%1:

    mov [bIRQShouldJump], byte 0

    ; Now, I could, and *used to* do all this
    ; overly complicated and error-prone code
    ; to retrieve the old eip from the stack,
    ; replace the one in the IRQ stack frame,
    ; then use "iret" to jump to the new code.
    ; Then one day I realised I was really
    ; overcomplicating things, and that actually,
    ; I could just switch tasks, and since the
    ; last thing in the stack was eip anyway,
    ; I could save myself considerable trouble
    ; (and deubgging) by just doing nothing.
    ; So if you're wondering where eip is getting
    ; set, wonder no more!

    ; Also, this should restore eflags and
    ; the cs register too, plus ss and esp
    call PerformTaskSwitch
    
FinishJump%1:
    iret    ; Return if PerformTaskSwitch wasn't called
%endmacro

; IRQs
IRQHandler 0
IRQHandler 1
IRQHandler 2
IRQHandler 3
IRQHandler 4
IRQHandler 5
IRQHandler 6
IRQHandler 7
IRQHandler 8
IRQHandler 9
IRQHandler 10
IRQHandler 11
IRQHandler 12
IRQHandler 13
IRQHandler 14
IRQHandler 15

; Unmapped IRQ handler
global IRQUnknown
IRQUnknown:
    cli

    ; Preserve state
    push eax
    push ebx
    push ecx
    push edx
    push ebp
    push edi
    push esi

    ; Preserve segment registers
    ; and use ring 0 ones
    push ds
    push es
    push fs
    push gs

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push    dword 1             ; This is an error! (well, sort of)
    push    dword 0xAF          ; Push superfluous id
    call    HandleInterrupts    ; Call generic function
    pop     eax
    pop     eax

    ; Get back segment registers
    pop gs
    pop fs
    pop es
    pop ds

    ; Restore state
    pop esi
    pop edi
    pop ebp
    pop edx
    pop ecx
    pop ebx
    pop eax

    iret                        ; Return

; IRQ exception handler
%macro IRQException 1
global IRQException%1
IRQException%1:
    cli

    ; Preserve segment registers
    ; and use ring 0 ones
    push ds
    push es
    push fs
    push gs

    push eax
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    pop eax

    ; Push registers
    push eax
    push ebx
    push ecx
    push edx
    push ebp
    push edi
    push esi
    
    ; Get error code and push to stack
    ; Some exceptions don't have an error
    ; code, but it's not like pushing
    ; an incorrect value breaks anything,
    ; and I'm all for simpler code if it's
    ; just for debugging anyway. If there
    ; isn't an error code, it'll just push
    ; the eip after all.
    mov eax, [esp+36]
    push eax

    ; Get eip and push to stack -
    ; this is often where the error occured!
    mov eax, [esp+44]
    push eax

    ; Push id to stack
    push dword %1

    call    HandleExceptions ; Call C code

    ; Unpop stack
    pop     eax
    pop     eax
    pop     eax

    ; Pop more off stack
    pop esi
    pop edi
    pop ebp
    pop edx
    pop ecx
    pop ebx
    pop eax

    ; Get back segment registers
    pop gs
    pop fs
    pop es
    pop ds

    iret                        ; Return
%endmacro

; IRQ exceptions
IRQException 0
IRQException 1
IRQException 2
IRQException 3
IRQException 4
IRQException 5
IRQException 6
IRQException 7
IRQException 8
IRQException 9
IRQException 10
IRQException 11
IRQException 12
IRQException 13
IRQException 14
IRQException 15
IRQException 16
IRQException 17
IRQException 18
IRQException 19
IRQException 20
IRQException 21
IRQException 22
IRQException 23
IRQException 24
IRQException 25
IRQException 26
IRQException 27
IRQException 28
IRQException 29
IRQException 30

; IRQ syscall 0x80
extern __tss_stack
extern HandleSyscalls
global IRQSyscall80
IRQSyscall80:
    cli

    ; When exiting tasks, the current
    ; stack becomes unallocated,
    ; so switch to the kernel stack
    ; instead (hardware TSS stuff is
    ; not used)
    cmp eax, 2
    je PerformTaskSave
    jmp AfterTaskSave

PerformTaskSave:
    mov esp, __tss_stack

AfterTaskSave:
    ; Preserve segment registers
    ; and use ring 0 ones
    push ds
    push es
    push fs
    push gs

    push eax
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    pop eax

    ; Preserve state
    push eax            ; (function argument)
    push ebx
    push ecx
    push edx
    push ebp
    push edi
    push esi
    
    call HandleSyscalls ; Call syscall
    
    pop esi
    pop edi
    pop ebp
    pop edx
    pop ecx
    pop ebx
    add esp, 4          ; Don't trash eax!

    ; Get back segment registers
    pop gs
    pop fs
    pop es
    pop ds

    ; Perform IRQ jump if nessecaryy
    cmp [bIRQShouldJump], byte 1
    je  SyscallIRQJump
    jmp SyscallFinishJump

SyscallIRQJump:
    mov [bIRQShouldJump], byte 0
    call PerformOneWaySwitch
SyscallFinishJump:
    ; Returun value is in eax,
    ; hence the failure to preserve
    ; it's value above - C expcects this
    iret                        ; Return