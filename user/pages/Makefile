NAME := pages

PROJDIRS := src
INCLUDEDIRS := -Iinclude -I../../kernel/include

CPPFILES := $(shell find $(PROJDIRS) -type f -name "*.cpp")
CFILES += $(shell find $(PROJDIRS) -type f -name "*.c")
HDRFILES := $(shell find $(PROJDIRS) -type f -name "*.h")

ASMFILES := $(shell find $(PROJDIRS) -type f -name "*.S")

OBJFILES += $(patsubst %.S,%.bin,$(ASMFILES))
OBJFILES := $(patsubst src/%,build/%,$(OBJFILES))

WARNINGS := -Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align \
			-Wwrite-strings -Wmissing-declarations \
			-Wredundant-decls -Winline -Wno-long-long \
			-Wconversion
CFLAGS := -std=gnu99 $(WARNINGS) -ffreestanding -O2 -nostdlib -lgcc
CPPFLAGS := -std=c++17 $(WARNINGS) -ffreestanding -ffreestanding -O2 -fno-exceptions -fno-rtti -nostdlib -libstdc++

TOOLCHAIN := i686-elf
ASSEMBLER := nasm

$(shell mkdir -p build)

all: $(OBJFILES)
	$(shell cp build/$(NAME).bin ../../build/isodir/modules/$(NAME))

build/%.bin: src/%.S
	@$(ASSEMBLER) -f bin $< -o $@

clean:
	-@$(RM) -r $(wildcard $(OBJFILES) build/*)