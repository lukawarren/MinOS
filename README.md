# MinOS
The third rewrite of MinOS

# Ports
* Musl 1.2.3
* Lua 5.4.4
* [Doom :-)](https://github.com/ozkl/doomgeneric)
* [Scalable Screen Font 2.0](https://gitlab.com/bztsrc/scalable-font2/)

## Building with Docker
This will build Musl too.
```
docker build -t minos:3.0 .
docker run --rm --volume $(pwd):/code minos:3.0
```

## Builing musl
```
cd userspace/musl &&
mkdir -p /tmp/musl-alias &&
ln -sf `which ar` /tmp/musl-alias/i686-ar &&
ln -sf `which ranlib` /tmp/musl-alias/i686-ranlib &&
export PATH=$PATH:"/tmp/musl-alias/" &&
CC=clang CFLAGS='--target=i686-pc-none-elf -march=i686 -DSYSCALL_NO_TLS' LDFLAGS='-fuse-ld=lld' ./configure --target=i686 &&
make -j 16
```

## Musl changes
* src/thread/i386/__set_thread_area.s has been commented out, and a stub provided in __init_tls.c instead (which avoids manipulating segment registers)
* arch/i386/pthred_arch.h has been stubbed out (for the same reason as above)
* crt/crt1.c supports the change above with a false pthread
* this all means that TLS is not supported yet

## Running with Qemu
`qemu-system-i386 -cdrom build/MinOS.iso -serial mon:stdio`

## Attributions
* Uses the [Gidole font](https://github.com/larsenwork/Gidole)

## History
* MinOS 1 - mostly written in the summer of 2021, C and C++
* MinOS 2 - written a few months ago (mid 2022), in Rust
* MinOS 3 - written now (August 2022), back in C and C++
