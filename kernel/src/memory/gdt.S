section text

gdtr DW 0 ; For limit storage
     DD 0 ; For base storage

global LoadGDT

LoadGDT:

    ; Load GDT
    mov   eax, [esp + 4]
    mov   [gdtr + 2], eax
    mov   ax, [esp + 8]
    mov   [gdtr], ax
    lgdt  [gdtr]

    ; Reload CS register containing code selector:
    jmp   0x08:ReloadCS ; 0x08 points at the new code selector

    ret
   
global ReloadCS

ReloadCS:

   ; Reload data segment registers:
   MOV   AX, 0x10 ; 0x10 points at the new data selector
   MOV   DS, AX
   MOV   ES, AX
   MOV   FS, AX
   MOV   GS, AX
   MOV   SS, AX

   RET