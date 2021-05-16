# MinOS
A x86 operating system written in modern(ish) C++, a bit of C, and Assembly.

## Features
* Userspace with standard library
* Pre-emptive multitaksing

## Depdendencies
If you want to build with Docker, then as it would turn out, all you need is:
* Docker

Building natively requires:
* A working i686 cross compiler ([see here for prebuilt binaries](https://github.com/lordmilko/i686-elf-tools))
* Grub 2.0
* NASM
* Make
* CMake

## Building with Docker
First download the project and build the toolchain with:
```
git clone https://github.com/TheUltimateKerbonaut/MinOS
cd MinOS
docker build -t minos/toolchain:1.0 .
```
Then compile the kernel:
```
docker run --rm  --volume /path/to/minos:/code minos/toolchain:1.0 bash -c "cd code && mkdir -p build && cd build && cmake .. && cmake --build ."
```

## Building natively
```
git clone https://github.com/lukawarren/MinOS
cd MinOS
mkdir -p build && cd build
cmake .. && cmake --build .
```

## Adding GRUB
After building the kernel binary file, run (in Docker if you must):
```
mkdir -p build/isodir/boot/grub
mkdir -p build/isodir/boot/userland
cp build/kernel/kernel.bin build/isodir/boot/MinOS.bin
cp build/userland/*/*.bin build/isodir/boot/userland/
cp kernel/grub.cfg build/isodir/boot/grub/grub.cfg
grub-mkrescue -o build/MinOS.iso build/isodir
```

## Running
Just build the project and either run the iso in the build directory as a virtual machine or burn to a disk with something like:
```
sudo dd if=/path/to/MinOS.iso of=/dev/sdx && sync
```

On Qemu:
```
qemu-system-i386 -cdrom build/MinOS.iso -serial stdio -vga std
```

## Newlibc (standard library)
For convenience, newlibc is already built, as it is a lengthy process and requires very specific versions of sotware (Automake 1.11 and Autoconf 2.65 to be exact).
If you really must build it for yourself, the following should work:

```
# 1) Link i686-elf toolchain to "i686-minos"
cd /path/to/toolchain/
ln i686-elf-ar i686-myos-ar
ln i686-elf-as i686-myos-as
ln i686-elf-gcc i686-myos-gcc
ln i686-elf-gcc i686-myos-cc
ln i686-elf-ranlib i686-myos-ranlib
cd /path/back/to/min/os/ 

# 2) Make build folder and build
cd lib/newlib-4.1.0
mkdir build
cd build
../configure --prefix=/minos --target=i686-minos
make -j4 all

# 3) Install to /tmp/minos then move to project
make DESTDIR=/tmp install
rm -r ../../i686-minos/
cp -r /tmp/minos/i686-minos/ ../../i686-minos/
```
