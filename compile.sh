cd build

# ASM
nasm -felf32 ../src/boot.asm -o boot.o
nasm -felf32 ../src/gdt/gdt.asm -o gdt_asm.o
nasm -felf32 ../src/idt/idt.asm -o idt_asm.o

# C
i386-elf-gcc -c ../src/std/stdout.c -o stdout.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
i386-elf-gcc -c ../src/gdt/gdt.c -o gdt_c.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
i386-elf-gcc -c ../src/idt/idt.c -o idt_c.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
i386-elf-gcc -c ../src/smbios/sysinfo.c -o sysinfo.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
i386-elf-gcc -c ../src/cmd/cmd.c -o cmd.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
i386-elf-gcc -c ../src/keyboard/keyboard.c -o keyboard.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
i386-elf-gcc -c ../src/kernel.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

i386-elf-gcc -c ../src/lua/lapi.c -o lua.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

# Linking
i386-elf-gcc -T ../linker.ld -o os.bin -ffreestanding -O2 -nostdlib boot.o stdout.o gdt_asm.o gdt_c.o idt_asm.o idt_c.o sysinfo.o cmd.o keyboard.o kernel.o -lgcc


cd ..
grub-file --is-x86-multiboot build/os.bin
if grub-file --is-x86-multiboot build/os.bin; then
  echo multiboot confirmed
else
  echo the file is not multiboot
fi


mkdir -p isodir/boot/grub
cp build/os.bin isodir/boot/os.bin
cp grub.cfg isodir/boot/grub/grub.cfg
grub-mkrescue -o os.iso isodir

sleep 1

qemu-system-i386 -cdrom os.iso
#bochs -f bochsrc.txt -q -rc bosh_cmd