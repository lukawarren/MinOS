include ../Makefile.variables

OUTPUT = build/minlibc.a
SOURCES = src/stdio.c src/syscalls.c
OBJECTS = $(SOURCES:src/%.c=build/%.o)
INCLUDES = $(SOURCES:src/%.c=include/%.h)

${OUTPUT}: $(OBJECTS)
	ar rcs $(OUTPUT) $(OBJECTS)

$(OBJECTS): $(SOURCES) $(INCLUDES)
	mkdir -p build
	${PREFIX}-gcc -c $(patsubst build/%.o, src/%.c, $@) -I include/ -o $@ $(RAW_CFLAGS)

clean:
	rm -rf build/
