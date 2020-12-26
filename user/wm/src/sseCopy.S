section text
global MemcpySSE
MemcpySSE:

    push edi
    push esi
    push ebx

    mov edi, [esp+16]    ; Destination
    mov esi, [esp+20]    ; Source

    mov ebx, [esp+24]   ; Size
    shr ebx, 7

    loop_copy:
        prefetchnta   [esi+128]
        prefetchnta   [esi+160]
        prefetchnta   [esi+192]
        prefetchnta   [esi+224]

        movdqa  xmm0, [esi+0]
        movdqa  xmm1, [esi+16]
        movdqa  xmm2, [esi+32]
        movdqa  xmm3, [esi+48]
        movdqa  xmm4, [esi+64]
        movdqa  xmm5, [esi+80]
        movdqa  xmm6, [esi+96]
        movdqa  xmm7, [esi+112]

        movntdq [edi+0],   xmm0
        movntdq [edi+16],  xmm1
        movntdq [edi+32],  xmm2
        movntdq [edi+48],  xmm3
        movntdq [edi+64],  xmm4
        movntdq [edi+80],  xmm5
        movntdq [edi+96],  xmm6
        movntdq [edi+112], xmm7

        add esi, 128
        add edi, 128
        dec ebx

        jnz loop_copy

    loop_copy_end:
        pop ebx
        pop esi
        pop edi
        ret
