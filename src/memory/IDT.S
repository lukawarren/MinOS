section text

global LoadIDT

LoadIDT:
    mov edx, [esp + 4]
    lidt [edx]
    sti ; enable interrupts
    ret
