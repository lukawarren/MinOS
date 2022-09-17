cmake_minimum_required(VERSION 3.1.3)
project(MinOS)

add_subdirectory(kernel)
add_subdirectory(userspace)

# GRUB 2
add_custom_target(run ALL DEPENDS kernel.bin DEPENDS minwm.bin doom.bin)
add_custom_command(TARGET run COMMAND
                    echo "Generating iso..." &&
                    mkdir -p isodir/boot/grub &&
                    cp ../grub.cfg isodir/boot/grub &&
                    cp kernel/kernel.bin isodir/boot &&
                    cp userspace/minwm/minwm.bin isodir/boot &&
                    cp userspace/doomgeneric/doom.bin isodir/boot &&
                    cp ../userspace/minwm/res/* isodir/boot &&
                    grub2-mkrescue -o MinOS.iso isodir &> /dev/null)
