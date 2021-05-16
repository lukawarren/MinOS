#include <stdlib.h>
#include <string.h>
#include <math.h>

int main()
{
    const char* sTest = "My number is";
    int length = strlen(sTest);
    return pow(2, length);
}