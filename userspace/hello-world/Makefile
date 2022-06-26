include ../Makefile.variables

NAME = hello-world
OUTPUT = ../../isodir/boot/$(NAME).module

all: $(OUTPUT)

$(OUTPUT): src/main.c
	mkdir -p build
	$(CC) -o build/main.o -c src/main.c $(CFLAGS)
	$(LD) -T ../linker.ld -o $(OUTPUT) build/main.o $(LINK_FLAGS)

clean:
	rm -rf build
	rm -f $(OUTPUT)
