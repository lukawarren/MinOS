gdtr DW 0 ; For limit storage
     DD 0 ; For base storage

global setGDT 
setGDT:
	MOV   EAX, [esp + 4]
	MOV   [gdtr + 2], EAX
	MOV   AX, [ESP + 8]
	MOV   [gdtr], AX

	cli
	LGDT  [gdtr]
	;sti

   	JMP 0x08:reload_CS
	
	; TSS
	mov ax, 0x18
	ltr ax

	RET

reload_CS:
	MOV   AX, 0x10 ; 0x10 points at the new data selector
	MOV   DS, AX
	MOV   ES, AX
	MOV   FS, AX
	MOV   GS, AX
	MOV   SS, AX

	RET