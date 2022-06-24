UNAME := $(shell uname)

all:
	# Toolchain
	cd toolchain && make all

	# Kernel
	cd kernel && make && \
	grub-file --is-x86-multiboot2 target/kernel.bin && \
	cp target/kernel.bin ../isodir/boot/kernel.bin

	# User programs
	cd userspace/minlibc && make
	cd userspace/libposix && make
	cd userspace/hello-world && make
	cd userspace/hello-world-c && make

	# ISO - GRUB needs the -d flag, unless compiled from source on MacOS (for me at least)
ifeq ($(UNAME), Darwin)
	grub-mkrescue -o MinOS.iso isodir/
else
	grub-mkrescue -d /usr/lib/grub/i386-pc -o MinOS.iso isodir/
endif

run: all
	qemu-system-i386 -cdrom MinOS.iso -serial stdio -vga std

clean:
	rm -f MinOS.iso
	rm -f isodir/boot/kernel.bin
	rm -f isodir/boot/*.module
	cd kernel && make clean
	cd userspace/minlibc && make clean
	cd userspace/libposix && make clean
	cd userspace/hello-world && make clean
	cd userspace/hello-world-c && make clean

clean-including-toolchain:
	rm -f MinOS.iso
	rm -f isodir/boot/kernel.bin
	rm -f isodir/boot/*.module
	cd toolchain && make clean
	cd kernel && make clean
	cd userspace/minlibc && make clean
	cd userspace/libposix && make clean
	cd userspace/hello-world && make clean
	cd userspace/hello-world-c && make clean

