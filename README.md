# MinOS
A tiny x86 kernel written in modern C++ and Assembly.

## Features
* Fully working usermode
* Multiboot 2 compliant kernel
* Interrupts
* Paging
* VGA output
* Serial UART output
* Pre-emptive multitaksing 
* Ramdisk for loading elf binaries in user space

## Depdendencies
If you want to build with Docker, then as it would turn out, all you need is:
* Docker

Building natively requires:
* A working i686 cross compiler ([see here for prebuilt binaries](https://github.com/lordmilko/i686-elf-tools))
* Grub 2.0
* NASM
* Make

## Building with Docker
First download the project and build the toolchain with:
```
git clone https://github.com/TheUltimateKerbonaut/MinOS
cd MinOS
docker build -t minos/toolchain:1.0 .
```
Then compile the kernel:
```
docker run --rm  --volume /path/to/minos:/code minos/toolchain:1.0 bash -c "cd code && make"
```

## Building natively
```
git clone https://github.com/TheUltimateKerbonaut/MinOS
cd MinOS
make
```

## Running
Just build the project and either run the iso in the build directory as a virtual machine or burn to a disk with something like:
```
sudo dd if=/path/to/MinOS.iso of=/dev/sdx && sync
```
