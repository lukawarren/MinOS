#include "cmd.h"

#include "../std/stdout.h"

#include "../idt/idt.h"

#include "../serial.h"

int strcmp(const char *X, const char *Y)
{
	while(*X)
	{
		// if characters differ or end of second string is reached
		if (*X != *Y)
			break;

		// move to next pair of characters
		X++;
		Y++;
	}

	// return the ASCII difference after converting char* to unsigned char*
	return (*(const unsigned char*)X - *(const unsigned char*)Y) == 0;
}

void parseCommand(const char* command)
{
    printf("");
    
    if (strcmp(command, "arthur"))
        printf("Is a communist, naturally :-)");
    
    else if (strcmp(command, "clear"))
        stdout_initialize();

    else if (strcmp(command, "panic"))
        endIQR(69);

    else if (strcmp(command, "suicide"))
    {
        for (unsigned int i = 0; i < 0xFFFFFFFF; ++i)
        {
            int* address = (int*)i;
            *address = 420;
        }
    }

    else if (strcmp(command, "help"))
        printf("Does this look like a charity to you?");

    else if (strcmp(command, "pi"))
        printf("bob");

    else
        printf("Command not found, commie!");
}