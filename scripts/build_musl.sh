#!/bin/bash
# Please run from the root directory :)
# Uses clang so you don't have to build a cross-compiler twice!

set -euo pipefail
pushd userspace/musl
mkdir -p /tmp/musl-alias
ln -sf `which ar` /tmp/musl-alias/i686-ar
ln -sf `which ranlib` /tmp/musl-alias/i686-ranlib
PATH=$PATH:"/tmp/musl-alias/"
CC=clang CFLAGS='--target=i686-pc-none-elf -march=i686 -DSYSCALL_NO_TLS' LDFLAGS='-fuse-ld=lld' ./configure --target=i686 --prefix=$PWD/sysroot
make -j 16
make install
popd
echo "Done :)"