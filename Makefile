NAME := MinOS

PROJDIRS := src
INCLUDEDIRS := -Iinclude -Iinclude/io

CPPFILES := $(shell find $(PROJDIRS) -type f -name "*.cpp")
CFILES += $(shell find $(PROJDIRS) -type f -name "*.c")
HDRFILES := $(shell find $(PROJDIRS) -type f -name "*.h")

ASMFILES := $(shell find $(PROJDIRS) -type f -name "*.S")

OBJFILES := $(patsubst %.cpp,%.o,$(CPPFILES))
OBJFILES += $(patsubst %.c,%.o,$(CFILES))
OBJFILES += $(patsubst %.S,%.o,$(ASMFILES))
OBJFILES := $(patsubst src/%,build/%,$(OBJFILES))

WARNINGS := -Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align \
            -Wwrite-strings -Wmissing-declarations \
            -Wredundant-decls -Winline -Wno-long-long \
            -Wconversion -Werror
CFLAGS := -std=gnu99 $(WARNINGS) -ffreestanding -O2 -nostdlib -lgcc
CPPFLAGS := -std=c++17 $(WARNINGS) -ffreestanding -ffreestanding -O2 -fno-exceptions -fno-rtti -nostdlib -libstdc++

TOOLCHAIN := i686-elf
QEMU := qemu-system-x86_64
ASSEMBLER := nasm

all: build/$(NAME).iso

build/$(NAME).iso: $(OBJFILES)
	echo $(OBJFILES)
	@$(TOOLCHAIN)-g++ -T src/linker.ld -o build/$(NAME).bin -ffreestanding -O2 -nostdlib $(OBJFILES) -lgcc
	mkdir -p build/isodir/boot/grub
	cp build/$(NAME).bin build/isodir/boot/$(NAME).bin
	cp src/grub.cfg build/isodir/boot/grub/grub.cfg
	grub-mkrescue -o build/$(NAME).iso build/isodir

build/%.o: src/%.cpp
	@$(TOOLCHAIN)-g++ $(CPPFLAGS) -c $< -o $@ $(INCLUDEDIRS)

build/%.o: src/%.c
	@$(TOOLCHAIN)-gcc $(CFLAGS) -c $< -o $@ -$(INCLUDEDIRS)

build/%.o: src/%.S
	@$(ASSEMBLER) -felf32 $< -o $@

clean:
	-@$(RM) -r $(wildcard $(OBJFILES) build/*)