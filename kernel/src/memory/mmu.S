section text

global LoadPageDirectories

LoadPageDirectories:
    push ebp
    mov ebp, esp
    mov eax, [esp + 8]
    mov cr3, eax
    mov esp, ebp
    pop ebp
    ret

global EnablePaging

EnablePaging:
    push ebp
    mov ebp, esp
    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax
    mov esp, ebp
    pop ebp
    ret

global FlushTLB
FlushTLB:
    ; On i486 and newer, the "invlpg" instruction
    ; makes much more sense, as what is below is
    ; overkill for most uses.
    push eax
    mov eax, cr3
    mov cr3, eax
    pop eax
    ret