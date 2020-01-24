global iretf
iretf:
	iretd
	RET

global setIDT
setIDT:
	mov eax, [esp+4]
	
	LIDT  [eax]
	sti
	RET