include ../Makefile.variables

NAME = cpu-idle
OUTPUT = ../../isodir/boot/$(NAME).module

all: $(OUTPUT)

$(OUTPUT): main.o
	$(LD) -T ../linker-assembly.ld -o $(OUTPUT) main.o

main.o: main.S
	nasm -f elf32 -o main.o main.S

clean:
	rm -f main.o
	rm -f $(OUTPUT)
