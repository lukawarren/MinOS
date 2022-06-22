DOWNLOAD_BINUTILS = binutils-2.38
DOWNLOAD_GCC = gcc-12.1.0
TARGET = i686-elf
PREFIX = $(shell pwd)/prefix
THREADS = 32

all: ${PREFIX}/bin/i686-elf-ld ${PREFIX}/bin/i686-elf-gcc

# binutils
${PREFIX}/bin/i686-elf-ld:
	curl http://ftp.gnu.org/gnu/binutils/${DOWNLOAD_BINUTILS}.tar.gz --output binutils.tar.gz
	tar -xzf binutils.tar.gz
	mkdir -p ${PREFIX}
	mkdir -p build/binutils
	cd build/binutils &&\
	../../${DOWNLOAD_BINUTILS}/configure --target=${TARGET} --prefix=${PREFIX} --with-sysroot --disable-multilib --disable-nls --disable-werror &&\
	make -j ${THREADS} &&\
	make install

# gcc
${PREFIX}/bin/i686-elf-gcc:
	curl http://ftp.gnu.org/gnu/gcc/${DOWNLOAD_GCC}/${DOWNLOAD_GCC}.tar.gz --output gcc.tar.gz
	tar -xzf gcc.tar.gz
	mkdir -p ${PREFIX}
	mkdir -p build/gcc
	cd build/gcc &&\
	../../${DOWNLOAD_GCC}/configure --target=${TARGET} --prefix=${PREFIX} --disable-multilib --disable-nls --enable-language=c,c++ --without-headers &&\
	make -j ${THREADS} all-gcc &&\
	make -j ${THREADS} all-target-libgcc &&\
	make -j ${THREADS} install-gcc &&\
	make -j ${THREADS} install-target-libgcc
	
# runs as normal but removes source files, etc after
keep-binaries-only: all
	rm -rf ${DOWNLOAD_BINUTILS}
	rm -rf ${DOWNLOAD_GCC}
	rm -rf build
	rm -f binutils.tar.gz
	rm -f gcc.tar.gz

clean:
	rm -r ${PREFIX}
	rm -f binutils.tar.gz
	rm -rf ${DOWNLOAD_BINUTILS}
	rm -f gcc.tar.gz
	rm -rf ${DOWNLOAD_GCC}
	rm -rf build