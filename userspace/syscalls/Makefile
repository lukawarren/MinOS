include ../Makefile.variables

all: syscalls.o

syscalls.o: build/posix.o
	ar -crs -o build/syscalls.a build/posix.o

build/posix.o: src/posix.c
	mkdir -p build
	$(CC) -o build/posix.o -c src/posix.c -I include/ $(CFLAGS)

clean:
	rm -rf build
