#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include  "../../../kernel/include/multitask/mman.h"

int main()
{
    mmap(0, 420, PROT_NONE, MAP_PRIVATE|MAP_NORESERVE|MAP_ANONYMOUS, -1, 0);
    return 0;
}