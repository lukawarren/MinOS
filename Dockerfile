FROM fedora:36

RUN dnf install -y clang g++ lld nasm binutils cmake ninja-build make xorriso grub2-tools-extra grub2-pc clang-tools-extra which

CMD cd /code &&\
    cd userspace/musl &&\
    mkdir -p /tmp/musl-alias &&\
    ln -sf `which ar` /tmp/musl-alias/i686-ar &&\
    ln -sf `which ranlib` /tmp/musl-alias/i686-ranlib &&\
    export PATH=$PATH:"/tmp/musl-alias/" &&\
    CC=clang CFLAGS='--target=i686-pc-none-elf -march=i686 -DSYSCALL_NO_TLS' LDFLAGS='-fuse-ld=lld' ./configure --target=i686 &&\
    make -j 16 &&\
    cd /code/ &&\
    mkdir -p build && cd build &&\
    cmake .. -DCMAKE_TOOLCHAIN_FILE=../toolchain/clang.cmake -DCMAKE_AR=$(which ar) -G Ninja &&\
    ninja