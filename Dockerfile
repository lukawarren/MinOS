FROM ubuntu:18.04

# Install dependencies
RUN apt-get update                                  && \
    # deps for gcc, binutils:
    apt-get install -y gcc wget build-essential     && \
    # deps for grub 2:
    apt-get install -y autoconf flex bison	    && \
    apt-get install -y curl python3 cpio ninja-build
	
# Specify binutils/gcc version
ENV DOWNLOAD_BINUTILS=binutils-2.35
ENV DOWNLOAD_GCC=gcc-10.2.0
ENV DOWNLOAD_GRUB=grub-2.02
ENV DOWNLOAD_XORRISO=xorriso-1.4.8

# Specify TARGET
ENV TARGET=i686-elf
ENV PREFIX=/usr/local

RUN echo "Beginning..."

# Binutils
RUN wget -q http://ftp.gnu.org/gnu/binutils/$DOWNLOAD_BINUTILS.tar.gz    && \
    tar -xzf $DOWNLOAD_BINUTILS.tar.gz                                   && \
    mkdir -p /srv/build_binutils                                         && \
    cd /srv/build_binutils                                               && \
    /$DOWNLOAD_BINUTILS/configure --target=$TARGET --prefix="$PREFIX"       \
    --with-sysroot --disable-multilib --disable-nls --disable-werror     && \
    make                                                                 && \
    make install                                                         && \
    rm -r /$DOWNLOAD_BINUTILS /srv/build_binutils

# GCC
RUN wget -q ftp://ftp.gnu.org/gnu/gcc/$DOWNLOAD_GCC/$DOWNLOAD_GCC.tar.gz && \
    tar -xzf $DOWNLOAD_GCC.tar.gz                                        && \
    cd /$DOWNLOAD_GCC && contrib/download_prerequisites                  && \
    mkdir -p /srv/build_gcc                                              && \
    cd /srv/build_gcc                                                    && \
    /$DOWNLOAD_GCC/configure --target=$TARGET --prefix="$PREFIX"            \
    --disable-multilib --disable-nls                                        \
    --enable-languages=c,c++ --without-headers                           && \
    make    all-gcc                                                      && \
    make    all-target-libgcc                                            && \
    make    install-gcc                                                  && \
    make    install-target-libgcc                                        && \
    rm -r /$DOWNLOAD_GCC /srv/build_gcc

# Xorriso
RUN wget -q ftp://ftp.gnu.org/gnu/xorriso/$DOWNLOAD_XORRISO.tar.gz       && \
    tar -xzf $DOWNLOAD_XORRISO.tar.gz                                    && \
    cd /$DOWNLOAD_XORRISO                                                && \
    ./configure --prefix=/usr                                            && \
    make                                                                 && \
    make install                                                         && \
    rm -r /$DOWNLOAD_XORRISO

# GRUB 2
RUN wget -q ftp://ftp.gnu.org/gnu/grub/$DOWNLOAD_GRUB.tar.gz             && \
    tar -xzf $DOWNLOAD_GRUB.tar.gz                                       && \
    cd /$DOWNLOAD_GRUB                                                   && \
    export PYTHON=python3                                                && \
    ./autogen.sh                                                         && \
    mkdir -p /srv/build_grub                                             && \
    cd /srv/build_grub                                                   && \
    /$DOWNLOAD_GRUB/configure --disable-werror TARGET_CC=$TARGET-gcc        \
    TARGET_OBJCOPY=$TARGET-objcopy TARGET_STRIP=$TARGET-strip               \
    TARGET_NM=$TARGET-nm TARGET_RANLIB=$TARGET-ranlib                       \
    --target=$TARGET                                                     && \
    make                                                                 && \
    make install                                                         && \
    rm -r /$DOWNLOAD_GRUB /srv/build_grub
	
# NASM
RUN apt-get install -y nasm

# Cmake
RUN apt-get install -y cmake

# cleanup
RUN apt-get clean autoclean                                              && \
    apt-get autoremove -y                                                && \
    rm -rf /var/lib/apt /var/lib/dpkg /var/lib/cache /var/lib/log

WORKDIR /