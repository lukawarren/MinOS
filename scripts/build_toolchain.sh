#!/bin/bash
# Please run from the root directory :)
# Script and toolchain heavily draws from https://github.com/tyler569/nightingale with patches from SerentiyOS too

set -euo pipefail
pushd toolchain

BINUTILS_VERSION="2.36.1"
GCC_VERSION="11.1.0"
PARALLEL="-j 16"

SYSROOT=$PWD/../userspace/musl/sysroot
PREFIX=$PWD/prefix
PATH="$PREFIX/bin:$PATH"
TARGET=i686-minos

# Clean up old builds
rm -rf binutils-build gcc-b binutils-${BINUTILS_VERSION} gcc-${GCC_VERSION}

# Download tar files
BINUTILS_TAR="binutils-${BINUTILS_VERSION}.tar.gz"
GCC_TAR="gcc-${GCC_VERSION}.tar.gz"
BINUTILS_DIR="binutils-${BINUTILS_VERSION}"
GCC_DIR="gcc-${GCC_VERSION}"

echo "Downloading binutils..."
[[ -f $BINUTILS_TAR ]] || wget -q https://ftp.gnu.org/gnu/binutils/binutils-${BINUTILS_VERSION}.tar.gz
echo "Downloading gcc..."
[[ -f $GCC_TAR ]] || wget -q https://ftp.gnu.org/gnu/gcc/gcc-${GCC_VERSION}/gcc-${GCC_VERSION}.tar.gz
tar xzf $BINUTILS_TAR
tar xzf $GCC_TAR

# Apply patches
pushd $BINUTILS_DIR
patch -p1 -i ../minos-binutils-${BINUTILS_VERSION}.patch
popd
pushd $GCC_DIR
patch -p1 -i ../minos-gcc-${GCC_VERSION}.patch
./contrib/download_prerequisites # GCC deps
popd

# Build binutils
mkdir -p binutils-build
pushd binutils-build
../$BINUTILS_DIR/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --disable-werror --with-sysroot="$SYSROOT"
make $PARALLEL
make install
popd

# Build gcc
mkdir -p gcc-build
pushd gcc-build
../$GCC_DIR/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --with-sysroot="$SYSROOT"
make $PARALLEL all-gcc && make $PARALLEL all-target-libgcc
make install-gcc
make install-target-libgcc
popd

popd
echo "Done :)"