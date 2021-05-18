#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static int scaryNonInitialisedMemory;

int main()
{
    return scaryNonInitialisedMemory;
}