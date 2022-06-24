NAME = libposix
OUTPUT = build/${NAME}.o
include ../Makefile.variables

all: main.c
	mkdir -p build
	$(PREFIX)-gcc $(MINLIBC_INCLUDE) -c main.c -o build/main.o $(RAW_CFLAGS)
	$(PREFIX)-ld -T $(LINKER_FILE) -relocatable -o $(OUTPUT) build/main.o

clean:
	rm -rf build
