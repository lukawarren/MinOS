all:
	# Kernel
	cd kernel && make && \
	grub-file --is-x86-multiboot2 target/kernel.bin && \
	cp target/kernel.bin ../isodir/boot/kernel.bin

	# User programs
	cd userspace/hello-world && make

	# ISO
	grub-mkrescue -d /usr/lib/grub/i386-pc -o MinOS.iso isodir/

run: all
	qemu-system-i386 -cdrom MinOS.iso -serial stdio -vga std

clean:
	rm -f MinOS.iso
	rm -f isodir/boot/kernel.bin
	cd kernel && make clean
	cd userspace/hello-world && make clean
