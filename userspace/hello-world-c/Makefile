NAME = hello-world-c
OUTPUT = ../../isodir/boot/${NAME}.module

LINKER_FILE = ../linker.ld
PREFIX = ../../toolchain/prefix/bin/i686-elf
LIBGCC = ../../toolchain/prefix/lib/gcc/i686-elf/12.1.0/libgcc.a
CFLAGS = -ffreestanding -nostdlib -O2 -Wall -Wextra
LINK_FLAGS = -nostdlib -O2 -L ${LIBGCC}

all: build/main.o
	${PREFIX}-ld -o ${OUTPUT} -T ${LINKER_FILE} ${LINK_FLAGS} build/main.o

build/main.o: main.c
	mkdir -p build
	${PREFIX}-gcc -c main.c -o build/main.o ${CFLAGS}

clean:
	rm -r build
	rm -f ${OUTPUT}