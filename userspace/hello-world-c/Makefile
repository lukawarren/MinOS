NAME = hello-world-c
OUTPUT = ../../isodir/boot/${NAME}.module
include ../Makefile.variables

all: build/main.o
	${PREFIX}-gcc -T ${LINKER_FILE} -o ${OUTPUT} build/main.o ${LINK_FLAGS}

build/main.o: main.c
	mkdir -p build
	${PREFIX}-gcc -c main.c -o build/main.o ${CFLAGS}

clean:
	rm -rf build
	rm -f ${OUTPUT}
