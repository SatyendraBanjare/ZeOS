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

void kernel_main(uint32_t ebx) {

	//initialize isr &irq to help input the 
	// working outputting to serial output . outb(0x3F8,'A');

	multiboot_info_t *mboot_ptr = (multiboot_info_t *) ebx;

	print_log("Initializing Descriptor Tables \n \n");
	init_descriptor_tables();

	
	print_log("Initializing Shell \n \n");
	init_shell();

	print_multiboot_info( mboot_ptr);



	

	if (mboot_ptr && (mboot_ptr->flags & MULTIBOOT_INFO_MODS) && (mboot_ptr->mods_count > 0))
	{
		zprint("Got Module count > 0 ");
	}

	
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
