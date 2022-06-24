include ../Makefile.variables

OUTPUT := minlibc.o
SRCDIR := functions
INCLUDES := -I ./include
SOURCES := $(shell find $(SRCDIR) -name '*.c' | grep -v _dlmalloc)
OBJECTS := $(SOURCES:%.c=%.o)

FLAGS := -D_PDCLIB_BUILD -D_PDCLIB_STATIC_DEFINE

all: $(OBJECTS)
	# Compile dlmalloc separately because it needs different includes
	${PREFIX}-gcc $(FLAGS) -I include/pdclib -I /usr/include -c functions/_dlmalloc/malloc.c -o functions/_dlmalloc/malloc.o $(RAW_CFLAGS) 
	ar rcs $(OUTPUT) $(OBJECTS) functions/_dlmalloc/malloc.o

$(OBJECTS): $(SOURCES)
	${PREFIX}-gcc $(FLAGS) $(INCLUDES) -c $(patsubst %.o, %.c, $@) -o $@ $(RAW_CFLAGS) 

clean:
	$(shell find . -name '*.o' | xargs rm)
