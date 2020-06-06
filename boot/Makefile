# AS = ~/build-i686-elf/linux/output/bin/i686-elf-as
CC = ~/build-i686-elf/linux/output/bin/i686-elf-gcc

#CC=~/build-x86_64-elf/linux/output/bin/x86_64-elf-gcc

AS= nasm

CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra


C_SOURCES = $(wildcard *.c)
ASM_SOURCES = $(wildcard *.asm)

OBJ = ${C_SOURCES:.c=.o} ${ASM_SOURCES:.asm=.o}

all: zeos.iso

%.o: %.c
	${CC} -c $< -o $@ $(CFLAGS)

# compile the boot and loader assembly files.
%.o: %.asm
	${AS} -felf32 $< -o $@

zeos.bin: ${OBJ}
	${CC} -T linker.ld -o $@ -ffreestanding -O2 -nostdlib $^ -lgcc

check-multiboot: zeos.bin
	grub-file --is-x86-multiboot zeos.bin

zeos.iso: check-multiboot
	rm -rf isodir/
	mkdir -p isodir/boot/grub
	cp zeos.bin isodir/boot/zeos.bin
	cp grub.cfg isodir/boot/grub/grub.cfg
	grub-mkrescue -o zeos.iso isodir

run:
	qemu-system-i386 -cdrom zeos.iso

clean:
	rm -rf isodir/
	rm -rf *.o
	rm -rf *.bin
	rm -rf	*.iso
