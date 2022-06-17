section text

; GDTR pointer storage
gdtr DW 0 ; limit
     DD 0 ; base

; IDT pointer storage
idtr DW 0 ; limit
     DD 0 ; base

global load_gdt
load_gdt:
    ; Load limit
    push eax
    mov ax, [esp]
    mov [gdtr], ax

    ; Load base
    mov eax, [esp + 8]
    mov [gdtr + 2], eax

    ; Send to GDT
    lgdt [gdtr]

    ; Reload CS register to
    ; allow us to change the
    ; code selector, by doing
    ; a "far jump"
    jmp 0x08:reload_cs

reload_cs:
    ; Setup selectors
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    pop eax
    ret


global load_idt
load_idt:
    ; Load limit
    push eax
    mov ax, [esp]
    mov [idtr], ax

    ; Load base
    mov eax, [esp + 8]
    mov [idtr + 2], eax

    ; Send to IDT
    lidt [idtr]

    pop eax
    ret