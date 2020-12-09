NAME := pages

PROJDIRS := src
INCLUDEDIRS := -Iinclude -I../../kernel/include

CPPFILES := $(shell find $(PROJDIRS) -type f -name "*.cpp")
CFILES += $(shell find $(PROJDIRS) -type f -name "*.c")
HDRFILES := $(shell find $(PROJDIRS) -type f -name "*.h")

ASMFILES := $(shell find $(PROJDIRS) -type f -name "*.S")

OBJFILES := $(patsubst %.cpp,%.cpp.o,$(CPPFILES))
OBJFILES += $(patsubst %.c,%.c.o,$(CFILES))
OBJFILES += $(patsubst %.S,%.S.o,$(ASMFILES))
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

build/%.cpp.o: src/%.cpp
	@$(TOOLCHAIN)-g++ $(CPPFLAGS) -c $< -o $@ $(INCLUDEDIRS)

build/%.c.o: src/%.c
	@$(TOOLCHAIN)-gcc $(CFLAGS) -c $< -o $@ $(INCLUDEDIRS)

build/%.S.o: src/%.S
	@$(ASSEMBLER) -felf32 $< -o $@

clean:
	-@$(RM) -r $(wildcard $(OBJFILES) build/*)