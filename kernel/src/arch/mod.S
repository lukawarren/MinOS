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


global load_tss
load_tss:
    push ax
    mov ax, [esp + 6] ; load descriptor
    ltr ax
    pop ax
    ret

global enable_paging
enable_paging:
    ; Modify cr0 to enable paging
    push ebp
    mov ebp, esp
    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax
    mov esp, ebp
    pop ebp
    ret


global flush_tlb
flush_tlb:
    ; On i486 and newer, the "invlpg" instruction
    ; makes much more sense, as what is below is
    ; overkill for most uses. However completely
    ; reloading cl3 should suffice for now until
    ; we run into any performance bottlenecks...
    push eax
    mov eax, cr3
    mov cr3, eax
    pop eax
    ret
