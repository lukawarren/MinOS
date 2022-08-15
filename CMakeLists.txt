cmake_minimum_required(VERSION 3.1.3)
project(MinOS)

add_subdirectory(kernel)

# GRUB 2
add_custom_target(run ALL DEPENDS kernel.bin)
add_custom_command(TARGET run COMMAND
                    mkdir -p isodir/boot/grub &&
                    cp ../grub.cfg isodir/boot/grub &&
                    cp kernel/kernel.bin isodir/boot &&
                    grub2-mkrescue -o MinOS.iso isodir &>/dev/null)
