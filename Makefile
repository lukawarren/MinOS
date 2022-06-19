all:
	# Kernel
	cd kernel && make && \
	grub-file --is-x86-multiboot2 target/kernel.bin && \
	cp target/kernel.bin ../isodir/boot/kernel.bin && \
	grub-mkrescue -d /usr/lib/grub/i386-pc -o ../MinOS.iso ../isodir

	# Modules
	cd modules/ps2 && make

run: all
	qemu-system-i386 -cdrom MinOS.iso -serial stdio -vga std

clean:
	rm MinOS.iso
	rm isodir/boot/kernel.bin
	cd kernel && make clean
	cd modules/ps2 && make clean
