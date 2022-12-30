#!/bin/bash
# Please run from the root directory :)
# Uses clang so you don't have to build a cross-compiler twice!

set -euo pipefail

SYSROOT="$PWD/userspace/musl/sysroot"
pushd userspace/musl

# Build musl with clang
mkdir -p /tmp/musl-alias
ln -sf `which ar` /tmp/musl-alias/i686-ar
ln -sf `which ranlib` /tmp/musl-alias/i686-ranlib
PATH=$PATH:"/tmp/musl-alias/"
CC=clang CFLAGS='--target=i686-pc-none-elf -march=i686 -DSYSCALL_NO_TLS' LDFLAGS='-fuse-ld=lld' ./configure --target=i686 --prefix=$SYSROOT
make -j 16

# Install headers to sysroot then copy files so GCC can find them
make install
pushd $SYSROOT
mkdir -p usr
cp -r bin usr/
cp -r include usr/
cp -r lib usr/
popd

popd
echo "Done :)"