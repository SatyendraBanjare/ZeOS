# # TARGET
# TARGET := x86_64

# # TOOLS ::
# # specify the cross-compiler and assembler sources
# #	download them from https://toolchains.bootlin.com/
# #	Simply pass the path to those files
# CC := ~/opt/x86-i686-tool/bin/i686-buildroot-linux-gnu-gcc
# LD := ~/opt/x86-i686-tool/bin/i686-buildroot-linux-gnu-ld
# AS := nasm
# AR := ~/opt/x86-64-tool/bin/x86_64-linux-ar
# QEMU := qemu-system-x86_64

# # Source Files

# # FLAGS

# # BUILD


# #####################
# # Display colors
# #####################
# BLUE		:= \033[1;34m
# YELLOW		:= \033[1;32m
# NORMAL		:= \033[0m

# SUCCESS := $(GREEN)[success]$(NORMAL)

###################################################################

# AS = ~/build-i686-elf/linux/output/bin/i686-elf-as
CC = ~/build-i686-elf/linux/output/bin/i686-elf-gcc

#CC=~/build-x86_64-elf/linux/output/bin/x86_64-elf-gcc

AS= nasm

CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra


C_SOURCES = $(wildcard kernel/*.c kernel/terminal/*.c)
ASM_SOURCES = $(wildcard boot/*.asm)

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
