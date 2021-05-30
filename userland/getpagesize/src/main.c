#include <stdio.h>
#include <unistd.h>

int main()
{
    int pageSize = getpagesize();
    return pageSize;
}