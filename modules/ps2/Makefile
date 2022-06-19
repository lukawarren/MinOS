MODULE_NAME = ps2
OUTPUT = ../../isodir/boot/${MODULE_NAME}.module

ASSEMBLER = nasm
ASSEMBLER_FLAGS = -f elf32
ASSEMBLY_SOURCES := $(wildcard */*.S) $(wildcard *.S)
ASSEMBLY_OBJECTS := $(patsubst %.S, %.o, $(ASSEMBLY_SOURCES))

LINKER_FILE = ../linker.ld

all: $(ASSEMBLY_OBJECTS)
	ld -m elf_i386 -o ${OUTPUT} -T ${LINKER_FILE} ${ASSEMBLY_OBJECTS}
	rm ${ASSEMBLY_OBJECTS}

$(ASSEMBLY_OBJECTS): $(ASSEMBLY_SOURCES)
	$(ASSEMBLER) $(ASSEMBLER_FLAGS) $(patsubst %.o, %.S, $@) -o $@

clean:
	rm ${OUTPUT}