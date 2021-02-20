#include "interrupts/syscall.h"
#include "stdlib.h"
#include "file/filedefs.h"

int main();

int main()
{
    char* filenameBuffer = (char*) malloc(1024);
    FileHandle file = getFirstFile();
    while ((signed int)file != -1)
    {
        getFileName(file, filenameBuffer);
        
        printf("--- ");
        printf(filenameBuffer);
        printf("\n");

        file = getNextFile(file);
    }

    free(filenameBuffer, 1024);

    sysexit();
    return 0;
}