void main(void)
{
    asm("xchg %bx, %bx");
    while(1) {}
}
