#include <stdio.h>
#include <stdlib.h>

void main(void)
{
    void* result = malloc(1024);
    printf("Hello world! I just malloc'd %d\n", (int)result);
    free(result);
    while(1) {}
}
