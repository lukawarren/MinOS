all:
	cd kernel && cargo build && make && \
	grub-file --is-x86-multiboot2 target/kernel.bin && \
	cp target/kernel.bin ../isodir/boot/kernel.bin && \
	grub-mkrescue -d /usr/lib/grub/i386-pc -o ../MinOS.iso ../isodir

run: MinOS.iso
	qemu-system-i386 -cdrom MinOS.iso -serial stdio -vga std

clean:
	rm MinOS.iso
	rm isodir/boot/kernel.bin
