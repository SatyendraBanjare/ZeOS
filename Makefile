# Pick the compiler based on the host OS.
# Prefer a bare-metal i686-elf cross compiler when available (required on macOS,
# optional on Linux); otherwise fall back to the host gcc in 32-bit mode.
UNAME_S := $(shell uname -s)
CROSS_CC := $(firstword $(wildcard /opt/homebrew/bin/i686-elf-gcc /usr/local/bin/i686-elf-gcc) $(shell command -v i686-elf-gcc 2>/dev/null))

ifneq ($(CROSS_CC),)
    CC = $(CROSS_CC)
    LDEXTRA =
    LIBGCC = -lgcc
    HOSTFLAGS =
else ifeq ($(UNAME_S),Linux)
    CC = gcc
    LDEXTRA = -m32 -no-pie
    LIBGCC =
    # host gcc defines __linux__, which trips the cross-compiler check in main.c
    HOSTFLAGS = -U__linux__
else
    $(error No suitable compiler for $(UNAME_S): install i686-elf-gcc (e.g. `brew install i686-elf-gcc`))
endif
AS= nasm

CFLAGS = -I/kernel/include/ -std=gnu99 -ffreestanding -O2 -Wall -Wextra -m32 -march=i386  -fPIC -fno-pie -fno-exceptions -fpermissive -fcommon $(HOSTFLAGS)

C_DIRS = kernel \
		 kernel/common \
		 kernel/cpu \
		 kernel/drivers \
		 kernel/fs \
		 kernel/terminal

C_SOURCES = $(shell find $(C_DIRS) -type f -name "*.c" )
ASM_SOURCES = $(wildcard boot/*.asm )

OBJ = ${ASM_SOURCES:.asm=.o} ${C_SOURCES:.c=.o} 

all: zeos.iso

%.o: %.c
	${CC} -c $(CFLAGS) $< -o $@ 

# compile the boot and loader assembly files.
%.o: %.asm
	${AS} -felf32 $< -o $@

zeos.bin: ${OBJ}
	${CC} -T conf/linker.ld -o $@ $(LDEXTRA) -ffreestanding -O2 -nostdlib $^ $(LIBGCC)

# check-multiboot: zeos.bin
# 	grub-file --is-x86-multiboot zeos.bin

# Pack everything under initrd/ into the ramdisk image loaded by GRUB
INITRD_FILES = $(shell find initrd -type f)

initrd.img: $(INITRD_FILES) scripts/make_initrd.py
	python3 scripts/make_initrd.py initrd $@

zeos.iso: zeos.bin initrd.img
	rm -rf isodir/
	rm -rf log/
	mkdir log
	touch log/log.txt
	mkdir -p isodir/boot/grub
	mkdir -p isodir/module
	cp zeos.bin isodir/boot/zeos.bin
	cp initrd.img isodir/module/initrd.img
	cp stage2_eltorito isodir/boot/grub/ 
	cp conf/menu.lst isodir/boot/grub/menu.lst
	mkisofs -R                              \
          -b boot/grub/stage2_eltorito    \
          -no-emul-boot                   \
          -boot-load-size 4               \
          -A os                           \
          -input-charset utf8             \
          -quiet                          \
          -boot-info-table                \
          -o zeos.iso                     \
          isodir
# 	cp conf/grub.cfg isodir/boot/grub/grub.cfg
#     grub-mkrescue -o zeos.iso isodir

run: zeos.iso
	qemu-system-i386 -m size=4096 -cdrom zeos.iso -serial file:log/log.txt

debug: zeos.iso #guest_errors,int,mmu,page,pcall
	qemu-system-i386 -m size=4096 -cdrom zeos.iso \
	-d guest_errors \
	-D debug.txt \
	-serial file:log/log.txt

clean:
	rm -rf isodir/
	rm -rf boot/*.o
	rm -rf boot/include/*.o
	rm -rf $(shell find $(C_DIRS) -type f -name "*.o" )
	rm -rf *.bin
	rm -rf	*.iso
	rm -rf log/
