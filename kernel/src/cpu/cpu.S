section text

gdtr DW 0 ; limit
     DD 0 ; base

global load_gdt
load_gdt:

    push eax

    ; Load GDT
    mov eax, [esp+8]
    mov [gdtr+2], eax
    mov ax, [esp+12]
    mov [gdtr], ax
    lgdt [gdtr]

    ; Reload CS register to change
    ; code selector to 0x8 via a
    ; far jump
    jmp 0x08:reload_cs
reload_cs:
    ; Now we have the right code
    ; selector, change data selectors
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    pop eax
    ret


global load_tss
load_tss:
    push ax
    mov ax, [esp + 6] ; Get descriptor

    ; Load it
    ltr ax

    pop ax
    ret


global load_idt
load_idt:
    ; Disable interrupts (just in case),
    ; load the IDT, then return
    cli
    push eax
    mov eax, [esp + 8]
    lidt [eax]
    pop eax
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