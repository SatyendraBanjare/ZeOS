CC = ~/build-i686-elf/linux/output/bin/i686-elf-gcc
AS= nasm

CFLAGS = -I/kernel/include/ -std=gnu99 -ffreestanding -O2 -Wall -Wextra -m32 -march=i386  -fPIC -fno-pie -fno-exceptions

C_DIRS = kernel \
		 kernel/common \
		 kernel/cpu \
		 kernel/drivers \
		 kernel/fs \
		 kernel/libc \
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
	${CC} -T conf/linker.ld -o $@ -ffreestanding -O2 -nostdlib $^ -lgcc

# check-multiboot: zeos.bin
# 	grub-file --is-x86-multiboot zeos.bin

zeos.iso: zeos.bin
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
	qemu-system-i386 -cdrom zeos.iso -serial stdio

debug:
	qemu-system-i386 -cdrom zeos.iso -d guest_errors,int &
	${GDB} -ex "target remote localhost:1234"

clean:
	rm -rf isodir/
	rm -rf boot/*.o
	rm -rf boot/include/*.o
	rm -rf $(shell find $(C_DIRS) -type f -name "*.o" )
	rm -rf *.bin
	rm -rf	*.iso
	rm -rf log/
