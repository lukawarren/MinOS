section text

global LoadTSS
extern ReloadCS

LoadTSS:
    
    mov dx, [esp + 4]

    mov ax, dx  ; TSS descriptor
    ltr ax      ; Load instruction
    
    jmp   0x08:ReloadCS ; 0x08 points at the code selector

    ret