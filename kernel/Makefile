RUST_LIBRARY = target/target/debug/libkernel.a

ASSEMBLER = nasm
ASSEMBLER_FLAGS = -f elf32
ASSEMBLY_SOURCES := $(wildcard src/*/*.S) $(wildcard src/*.S)
ASSEMBLY_OBJECTS := $(patsubst %.S, %.o, $(ASSEMBLY_SOURCES))

LINKER_FILE = src/linker.ld
LINKER = ../toolchain/prefix/bin/i686-elf-ld

all: $(ASSEMBLY_OBJECTS)
	cargo build
	${LINKER} -o target/kernel.bin -T ${LINKER_FILE} ${ASSEMBLY_OBJECTS} $(RUST_LIBRARY)
	rm ${ASSEMBLY_OBJECTS}

$(ASSEMBLY_OBJECTS): $(ASSEMBLY_SOURCES)
	$(ASSEMBLER) $(ASSEMBLER_FLAGS) $(patsubst %.o, %.S, $@) -o $@
	
clean:
	cargo clean
