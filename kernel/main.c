#include "include/multiboot.h"
#include "include/cpu/gdt.h"
#include "include/cpu/idt.h"
#include "include/cpu/isr.h"
#include "include/terminal/terminal.h"
#include "include/common/helper.h"

#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

//check flag for if username & password are correct
int shell_access_flag = 0;

void init_descriptor_tables(){
	//initialize GDT table
	init_gdt();

	//initialize IDT table
	isr_install();
	irq_install();
}

void kernel_main(struct multiboot_info *mboot_ptr) {

	//initialize isr &irq to help input the 
	init_descriptor_tables();

	init_shell();

	if (mboot_ptr && (mboot_ptr->flags & MULTIBOOT_INFO_MODS) && (mboot_ptr->mods_count > 0))
	{
		zprint("Got Module count > 0 ");
	}

	uint32_t initrd_location = *((uint32_t*)mboot_ptr->mods_addr);

	zprint(int_to_str(initrd_location));

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
