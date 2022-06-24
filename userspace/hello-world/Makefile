NAME = hello-world
OUTPUT = ../../isodir/boot/${NAME}.module

ASSEMBLER = nasm
ASSEMBLER_FLAGS = -f elf32
ASSEMBLY_SOURCES := $(wildcard */*.S) $(wildcard *.S)
ASSEMBLY_OBJECTS := $(patsubst %.S, %.o, $(ASSEMBLY_SOURCES))

PREFIX = ../../toolchain/prefix/bin/i686-elf
LINKER_FILE = ../linker.ld

all: $(ASSEMBLY_OBJECTS)
	${PREFIX}-ld -o ${OUTPUT} -T ${LINKER_FILE} ${ASSEMBLY_OBJECTS}
	rm ${ASSEMBLY_OBJECTS}

$(ASSEMBLY_OBJECTS): $(ASSEMBLY_SOURCES)
	$(ASSEMBLER) $(ASSEMBLER_FLAGS) $(patsubst %.o, %.S, $@) -o $@

clean:
	rm -f ${OUTPUT}
