#include "../include/multiboot/multiboot_util.h"

uint32_t p_to_v(uint32_t physical_address) {
  return physical_address + (uint32_t) &KERNEL_VIRTUAL_BASE;
}

void print_mem_type(int N){
	if(N == 1){print_log(" 1 MULTIBOOT_MEMORY_AVAILABLE");}
	else if(N == 2){print_log(" 2 MULTIBOOT_MEMORY_RESERVED");}
};


void print_memory_map(struct multiboot_info* info) {

  print_log( "\nmemory map:\n");
  multiboot_memory_map_t * memory_map = (multiboot_memory_map_t *) p_to_v(info->mmap_addr);
  uint32_t num_entries = info->mmap_length / sizeof(multiboot_memory_map_t);

  for (uint32_t i = 0; i < num_entries; i++) {
    print_log( "address: ");
    print_log("0x");print_log_int( memory_map[i].addr,16);
    print_log( "\n");

    print_log( "length: 0x");
    print_log_int( memory_map[i].len,16);
    print_log( "\n");

	print_log( "size: ");
    print_log("0x");print_log_int( memory_map[i].size,16);
    print_log( "\n");

    print_log( "type: ");
    print_mem_type( memory_map[i].type);
    print_log( "\n");

    print_log( "\n");
  }
}

void print_module_info(struct multiboot_info * info) {

	print_log("GRUB Modules : ");

	if(info->mods_count > 0 ){

		for(int i = 0; i < (info->mods_count) ; i++){
			multiboot_module_t * module = (multiboot_module_t *) p_to_v(info->mods_addr);

			print_log( (char *)(p_to_v(module->cmdline)) );   print_log("\n");
			print_log("Mod Start (Virtual) : 0x"); print_log_int(p_to_v(module->mod_start),16);  print_log("\n");
			print_log("Mod End (Virtual) : 0x"); print_log_int(p_to_v(module->mod_end),16);  print_log("\n");

			print_log("\n \n");
		}
	}else{print_log("N0 GRUB Modules Found !!");}

}

void print_multiboot_info(struct multiboot_info *mboot_ptr){

	//print extra 0x for hex string

	print_log("flags : 0x");			print_log_int(mboot_ptr->flags,16);					print_log("\n");
	print_log("mem_lower (KB) : ");		print_log_int(mboot_ptr->mem_lower,10);				print_log("\n");
	print_log("mem_upper (KB) : ");		print_log_int(mboot_ptr->mem_upper,10);				print_log("\n");
	print_log("boot_device : 0x");		print_log_int(mboot_ptr->boot_device,16);			print_log("\n");
	print_log("cmdline :");				print_log((char *)(p_to_v(mboot_ptr->cmdline)));	print_log("\n");
	print_log("mods_count : ");			print_log_int(mboot_ptr->mods_count,10);			print_log("\n");
	print_log("mods_addr : 0x");		print_log_int(p_to_v(mboot_ptr->mods_addr),16);				print_log("\n");

	print_log("\n \n");
	print_log("Flags Set :");
	print_log("\n \n");

	if (mboot_ptr && (mboot_ptr->flags & MULTIBOOT_INFO_MEMORY) )
	{
		print_log("MULTIBOOT_INFO_MEMORY  ");print_log("\n");
	}
	if (mboot_ptr && (mboot_ptr->flags & MULTIBOOT_INFO_BOOTDEV) )
	{
		print_log("MULTIBOOT_INFO_BOOTDEV ");print_log("\n");
	}
	if (mboot_ptr && (mboot_ptr->flags & MULTIBOOT_INFO_CMDLINE) )
	{
		print_log("MULTIBOOT_INFO_CMDLINE  ");print_log("\n");
	}
	if (mboot_ptr && (mboot_ptr->flags & MULTIBOOT_INFO_MODS) )
	{
		print_log("Got MULTIBOOT_INFO_MODS  ");print_log("\n");
	}
	if (mboot_ptr && (mboot_ptr->flags & MULTIBOOT_INFO_AOUT_SYMS) )
	{
		print_log("Got MULTIBOOT_INFO_AOUT_SYMS  ");print_log("\n");
	}
	if (mboot_ptr && (mboot_ptr->flags & MULTIBOOT_INFO_ELF_SHDR) )
	{
		print_log("Got MULTIBOOT_INFO_ELF_SHDR  ");print_log("\n");
	}

	if (mboot_ptr && (mboot_ptr->flags & MULTIBOOT_INFO_MEM_MAP) )
	{
		print_log("Got MULTIBOOT_INFO_MEM_MAP  ");print_log("\n");
	}
	if (mboot_ptr && (mboot_ptr->flags & MULTIBOOT_INFO_DRIVE_INFO) )
	{
		print_log("Got MULTIBOOT_INFO_DRIVE_INFO  ");print_log("\n");
	}
	if (mboot_ptr && (mboot_ptr->flags & MULTIBOOT_INFO_CONFIG_TABLE) )
	{
		print_log("Got MULTIBOOT_INFO_CONFIG_TABLE  ");print_log("\n");
	}
	if (mboot_ptr && (mboot_ptr->flags & MULTIBOOT_INFO_BOOT_LOADER_NAME) )
	{
		print_log("Got MULTIBOOT_INFO_BOOT_LOADER_NAME  ");print_log("\n");
	}
	if (mboot_ptr && (mboot_ptr->flags & MULTIBOOT_INFO_APM_TABLE) )
	{
		print_log("Got MULTIBOOT_INFO_APM_TABLE  ");print_log("\n");
	}
	if (mboot_ptr && (mboot_ptr->flags & MULTIBOOT_INFO_VBE_INFO) )
	{
		print_log("Got MULTIBOOT_INFO_VBE_INFO  ");print_log("\n");
	}
	if (mboot_ptr && (mboot_ptr->flags & MULTIBOOT_INFO_FRAMEBUFFER_INFO) )
	{
		print_log("Got MULTIBOOT_INFO_FRAMEBUFFER_INFO  ");print_log("\n");
	}

	// if (mboot_ptr->mods_count > 0){print_log(" MMMM module count >0");}

	print_log("\n \n");

	print_memory_map(mboot_ptr);

	print_module_info(mboot_ptr);
}
