NAME := MinOS

all: build/$(NAME).iso

build/$(NAME).iso:
	$(MAKE) -C kernel

	mkdir -p build/isodir/boot/grub
	cp kernel/build/$(NAME).bin build/isodir/boot/$(NAME).bin
	cp kernel/src/grub.cfg build/isodir/boot/grub/grub.cfg
	grub-mkrescue -o build/$(NAME).iso build/isodir

clean:
	$(MAKE) -C kernel clean