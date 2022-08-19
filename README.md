# MinOS
The third rewrite of MinOS

# Builing musl
```
cd musl &&
mkdir -p /tmp/musl-alias &&
ln -sf `which ar` /tmp/musl-alias/i686-ar &&
ln -sf `which ranlib` /tmp/musl-alias/i686-ranlib &&
export PATH=$PATH:"/tmp/musl-alias/" &&
CC=clang CFLAGS='--target=i686-pc-none-elf -march=i686 -DSYSCALL_NO_TLS' LDFLAGS='-fuse-ld=lld' ./configure --target=i686 &&
make -j 16
```

## History
* MinOS 1 - mostly written in the summer of 2021, C and C++
* MinOS 2 - written a few months ago (mid 2022), in Rust
* MinOS 3 - written now (August 2022), back in C and C++
