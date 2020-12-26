section text

global IsSSESupported
IsSSESupported:
    push ebx ; cpuid modifies
    push ecx ; ebx, ecx and
    push edx ; edx  
    
TestForSSE:
    mov eax, 1
    cpuid
    test edx, 1 << 25 ; SSE
    jz noSSE

TestForSSE2:
    ; SSE supported
    ; But what about SSE2?
    test edx, 1 << 26 ; SSE 2
    jz noSSE

TestForFXSR: ; FXRSTOR and FXSAVE instructions
    test edx, 1 << 24
    jz noSSE

    pop ebx
    pop ecx
    pop edx
    ret


noSSE:
    ; Return 0
    mov eax, 0

    pop ebx
    pop ecx
    pop edx
    ret

global EnableSSE
EnableSSE:

    ; Setup CR0
    mov eax, cr0
    and ax, 0xFFFB  ; clear coprocessor emulation - CR0.EM
    or ax, 0x2      ; set coprocessor monitoring  - CR0.MP
    mov cr0, eax

    ; Setup CR4
    mov eax, cr4
    or ax, 3 << 9   ; set CR4.OSFXSR and CR4.OSXMMEXCPT
    mov cr4, eax

    ret