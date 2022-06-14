RUST_LIBRARY = target/target/debug/libkernel.a

ASSEMBLER = nasm
ASSEMBLER_FLAGS = -f elf32
ASSEMBLY_SOURCES := $(wildcard src/*/*.S) $(wildcard src/*.S)
ASSEMBLY_OBJECTS := $(patsubst src/%.S, target/%.o, $(ASSEMBLY_SOURCES))

LINKER_FILE = src/linker.ld

all: $(ASSEMBLY_OBJECTS)
	ld -m elf_i386 -o target/kernel.bin -T ${LINKER_FILE} ${ASSEMBLY_OBJECTS} $(RUST_LIBRARY)

$(ASSEMBLY_OBJECTS): $(ASSEMBLY_SOURCES)
	$(ASSEMBLER) $(ASSEMBLER_FLAGS) $< -o $@
	
clean:
	$(R_COMPILER) clean
	rm target/kernel.bin