NAME := MinOS

all: build/$(NAME).iso

build/$(NAME).iso:

	mkdir -p build/isodir/modules/

	$(MAKE) -C stdlib
	$(MAKE) -C kernel
	$(MAKE) -C user/pages

	cp scripts/filesystem/output/filesystem.bin build/isodir/modules/filesystem.bin

	mkdir -p build/isodir/boot/grub
	cp kernel/build/$(NAME).bin build/isodir/boot/$(NAME).bin
	cp kernel/src/grub.cfg build/isodir/boot/grub/grub.cfg
	grub-mkrescue -o build/$(NAME).iso build/isodir

clean:
	-@$(RM) -r $(wildcard $(OBJFILES) build/*)
	
	$(MAKE) -C stdlib clean
	$(MAKE) -C kernel clean
	$(MAKE) -C user/pages clean