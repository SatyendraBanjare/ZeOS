CC = ~/build-i686-elf/linux/output/bin/i686-elf-gcc
AS= nasm

CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra

C_SOURCES = $(wildcard kernel/*.c kernel/terminal/*.c)
ASM_SOURCES = $(wildcard boot/*.asm boot/include/*.asm)

OBJ = ${C_SOURCES:.c=.o} ${ASM_SOURCES:.asm=.o}

all: zeos.iso

%.o: %.c
	${CC} -c $< -o $@ $(CFLAGS)

# compile the boot and loader assembly files.
%.o: %.asm
	${AS} -i/boot/include -felf32 $< -o $@

zeos.bin: ${OBJ}
	${CC} -T misc/linker.ld -o $@ -ffreestanding -O2 -nostdlib $^ -lgcc

check-multiboot: zeos.bin
	grub-file --is-x86-multiboot zeos.bin

zeos.iso: check-multiboot
	rm -rf isodir/
	mkdir -p isodir/boot/grub
	cp zeos.bin isodir/boot/zeos.bin
	cp misc/grub.cfg isodir/boot/grub/grub.cfg
	grub-mkrescue -o zeos.iso isodir

run:
	qemu-system-i386 -cdrom zeos.iso

clean:
	rm -rf isodir/
	rm -rf boot/*.o
	rm -rf kernel/*.o
	rm -rf kernel/terminal/*.o
	rm -rf *.bin
	rm -rf	*.iso
