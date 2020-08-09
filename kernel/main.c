#include "include/multiboot/multiboot.h"
#include "include/cpu/gdt.h"
#include "include/cpu/idt.h"
#include "include/cpu/isr.h"
#include "include/terminal/terminal.h"
#include "include/common/helper.h"
#include "include/multiboot/multiboot_util.h"

#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

struct kernel_memory_descriptor_t {
  uint32_t kernel_virtual_start;
  uint32_t kernel_virtual_end;
  uint32_t kernel_physical_start;
  uint32_t kernel_physical_end;
};



//check flag for if username & password are correct
int shell_access_flag = 0;

uint32_t phys_addr_start;
uint32_t phys_addr_end;


void init_descriptor_tables(){
	//initialize GDT table
	init_gdt();

	//initialize IDT table
	isr_install();
	irq_install();
}

void kernel_main(struct kernel_memory_descriptor_t kernel_memory, uint32_t ebx) {

	//initialize isr &irq to help input the 
	// working outputting to serial output . outb(0x3F8,'A');

	multiboot_info_t *mboot_ptr = (multiboot_info_t *) ebx;

	print_log("Initializing Descriptor Tables \n \n");
	init_descriptor_tables();

	
	print_log("Initializing Shell \n \n");
	init_shell();

	print_multiboot_info( mboot_ptr);



	

	if (mboot_ptr && (mboot_ptr->flags & MULTIBOOT_INFO_MEMORY) )
	{
		zprint("Got memory  ");zprint("\n");
	}
	if (mboot_ptr && (mboot_ptr->flags & MULTIBOOT_INFO_BOOTDEV) )
	{
		zprint("Got bootdev  ");zprint("\n");
	}
	if (mboot_ptr && (mboot_ptr->flags & MULTIBOOT_INFO_CMDLINE) )
	{
		zprint("Got cmdline  ");zprint("\n");
	}
	if (mboot_ptr && (mboot_ptr->flags & MULTIBOOT_INFO_MODS) )
	{
		zprint("Got MULTIBOOT_INFO_MODS  ");zprint("\n");
	}
	if (mboot_ptr && (mboot_ptr->flags & MULTIBOOT_INFO_AOUT_SYMS) )
	{
		zprint("Got MULTIBOOT_INFO_AOUT_SYMS  ");zprint("\n");
	}
	if (mboot_ptr && (mboot_ptr->flags & MULTIBOOT_INFO_ELF_SHDR) )
	{
		zprint("Got MULTIBOOT_INFO_ELF_SHDR  ");zprint("\n");
	}

	if (mboot_ptr && (mboot_ptr->flags & MULTIBOOT_INFO_MEM_MAP) )
	{
		zprint("Got MULTIBOOT_INFO_MEM_MAP  ");zprint("\n");
	}
	if (mboot_ptr && (mboot_ptr->flags & MULTIBOOT_INFO_DRIVE_INFO) )
	{
		zprint("Got MULTIBOOT_INFO_DRIVE_INFO  ");zprint("\n");
	}
	if (mboot_ptr && (mboot_ptr->flags & MULTIBOOT_INFO_CONFIG_TABLE) )
	{
		zprint("Got MULTIBOOT_INFO_CONFIG_TABLE  ");zprint("\n");
	}
	if (mboot_ptr && (mboot_ptr->flags & MULTIBOOT_INFO_BOOT_LOADER_NAME) )
	{
		zprint("Got MULTIBOOT_INFO_BOOT_LOADER_NAME  ");zprint("\n");
	}
	if (mboot_ptr && (mboot_ptr->flags & MULTIBOOT_INFO_APM_TABLE) )
	{
		zprint("Got MULTIBOOT_INFO_APM_TABLE  ");zprint("\n");
	}
	if (mboot_ptr && (mboot_ptr->flags & MULTIBOOT_INFO_VBE_INFO) )
	{
		zprint("Got MULTIBOOT_INFO_VBE_INFO  ");zprint("\n");
	}
	if (mboot_ptr && (mboot_ptr->flags & MULTIBOOT_INFO_FRAMEBUFFER_INFO) )
	{
		zprint("Got MULTIBOOT_INFO_FRAMEBUFFER_INFO  ");zprint("\n");
	}


	if (mboot_ptr->mods_count > 0){zprint("module count >0");}

	
	// User Acces Stuff
	// while(shell_access_flag == 0){
	// 	if(has_access() == 1)
	// 	{
	// 		shell_access_flag = 1;
	// 		start_execution();
	// 	}
	// }

	// start_execution();


}
