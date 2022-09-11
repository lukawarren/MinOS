#include "font.h"
extern "C" { int main(); }

int main()
{
    init_font("Gidole-Regular.sfn", 64);
    draw_font("Hello world!", 0xffffffff, 100, 200);
    free_font();

    volatile int hang = 1;
    while(hang) { hang = 1; }
    return 0;
}
