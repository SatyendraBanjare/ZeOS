#include "../include/multiboot/multiboot_util.h"

char *itoa(int n, char *str, int base) {
    if (base < 2 || base > 16) {
        str[0] = '\0';
        return str;
    }
    char alph[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' }; // Alphabet
    int i, sign;
    if (base == 10 && (sign = n) < 0) n = -n; // Signed mode is useful just when working in base 10
    unsigned int m = (unsigned int)n; // Unsigned copy of n, to make calculations
    i = 0;
    do {
        str[i++] = alph[m % base];
    } while ((m /= base) > 0);
    if (base == 10 && sign < 0) str[i++] = '-'; // Signed mode is useful just when working in base 10
    str[i] = '\0';
    reverse(str);
    return str;
}


void print_multiboot_info(struct multiboot_info *mboot_ptr){

	//print extra 0x for hex string

	char flags[10];
	char mem_lower[10];
	char mem_upper[10];
	char boot_device[10];
	char cmdline[10];
	char mods_count[10];
	char mods_addr[10];

	itoa(mboot_ptr->flags,flags,16);
	itoa(mboot_ptr->mem_lower,mem_lower,10);
	itoa(mboot_ptr->mem_upper,mem_upper,10);
	itoa(mboot_ptr->boot_device,boot_device,16);
	itoa(mboot_ptr->cmdline,cmdline,16);
	itoa(mboot_ptr->mods_count,mods_count,10);
	itoa(mboot_ptr->mods_addr,mods_addr,16);

  // fprintf(stream, "flags: %x\n", mboot_ptr->flags);
  // fprintf(stream, "mem_lower (kilobytes): %i\n", mboot_ptr->mem_lower);
  // fprintf(stream, "mem_upper (kilobytes): %i\n", mboot_ptr->mem_upper);
  // fprintf(stream, "boot_device: %x\n", mboot_ptr->boot_device);
  // fprintf(stream, "cmdline: %x\n", mboot_ptr->cmdline);
  // fprintf(stream, "mods_count: %i\n", mboot_ptr->mods_count); //base 10
  // fprintf(stream, "mods_addr (virtual): %x\n", mboot_ptr->mods_addr);
	
	zprint("flags : 0x");zprint(flags);zprint("\n");
	zprint("mem_lower (KB) : ");zprint(mem_lower);zprint("\n");
	zprint("mem_upper (KB) : ");zprint(mem_upper);zprint("\n");
	zprint("boot_device : 0x");zprint(boot_device);zprint("\n");
	zprint("cmdline : 0x");zprint(cmdline);zprint("\n");
	zprint("mods_count : ");zprint(mods_count);zprint("\n");
	zprint("mods_addr : 0x");zprint(mods_addr);zprint("\n");

	zprint("\n \n \n");
}
