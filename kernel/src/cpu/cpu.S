section text

gdtr DW 0 ; Store limit
     DD 0 ; Store base

global LoadGDT
LoadGDT:

    ; Load GDT
    mov eax, [esp+4]
    mov [gdtr+2], eax
    mov ax, [esp+8]
    mov [gdtr], ax
    lgdt [gdtr]

    ; Reload CS register to change
    ; code selector to 0x8 via a
    ; far jump
    jmp 0x08:ReloadCS

ReloadCS:

    ; Now we have the right code
    ; selector, change data selectors
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    ret

global LoadIDT
LoadIDT:
    ; Disable interrupts (just in case),
    ; load the IDT, then return
    cli
    push eax
    mov eax, [esp + 8]
    lidt [eax]
    pop eax
    ret

global FlushTLB
FlushTLB:
    ; On i486 and newer, the "invlpg" instruction
    ; makes much more sense, as what is below is
    ; overkill for most uses. However completely
    ; reloading cl3 does at least alleviate
    ; my paranoia
    push eax
    mov eax, cr3
    mov cr3, eax
    pop eax
    ret

global EnablePaging
EnablePaging:
    ; Modify cr0 to enable paging
    push ebp
    mov ebp, esp
    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax
    mov esp, ebp
    pop ebp
    ret

global LoadPageDirectories
LoadPageDirectories:
    ; Put the address of the page
    ; directories into cr3
    push ebp
    mov ebp, esp
    mov eax, [esp + 8]
    mov cr3, eax
    mov esp, ebp
    pop ebp
    ret