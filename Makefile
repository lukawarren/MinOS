all:
	cd kernel && make && \
	grub-file --is-x86-multiboot2 target/kernel.bin && \
	cp target/kernel.bin ../isodir/boot/kernel.bin && \
	grub-mkrescue -d /usr/lib/grub/i386-pc -o ../MinOS.iso ../isodir
	
clean:
	rm MinOS.iso
	rm isodir/boot/kernel.bin