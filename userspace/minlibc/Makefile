include ../Makefile.variables

OUTPUT := build/minlibc.o
SRCDIR := functions
BUILDDIR := build
INCLUDES := -I build/include -I /usr/include # need /usr/include for <features.h> defined in /usr/include/errno.h
SOURCES := $(shell find $(SRCDIR) -name '*.c' | grep -v _dlmalloc)
OBJECTS := $(addprefix $(BUILDDIR)/,$(SOURCES:%.c=%.o))

FLAGS := -D_PDCLIB_BUILD -D_PDCLIB_STATIC_DEFINE

all: $(OBJECTS)

	# Patch the source
	mkdir -p build
	cp -r functions $(BUILDDIR)/
	cp -r include $(BUILDDIR)/
	cp -r minos/functions $(BUILDDIR)/
	cp -r minos/include $(BUILDDIR)/

	# Compile dlmalloc separately because it needs different includes
	${PREFIX}-gcc $(FLAGS) -I minos/include/pdclib -I /usr/include -c build/functions/_dlmalloc/malloc.c -o build/functions/_dlmalloc/malloc.o $(RAW_CFLAGS) 
	${PREFIX}-ld -T $(LINKER_FILE) -relocatable -o $(OUTPUT) $(OBJECTS) build/functions/_dlmalloc/malloc.o

$(OBJECTS): $(SOURCES)

	# Patch the source
	mkdir -p build
	cp -r functions $(BUILDDIR)/
	cp -r include $(BUILDDIR)/
	cp -r minos/functions $(BUILDDIR)/
	cp -r minos/include $(BUILDDIR)/

	${PREFIX}-gcc $(FLAGS) $(INCLUDES) -c $(patsubst %.o, %.c, $@) -o $@ $(RAW_CFLAGS) 

clean:
	rm -rf build