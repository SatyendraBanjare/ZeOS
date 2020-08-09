#include "include/multiboot/multiboot.h"
#include "include/cpu/gdt.h"
#include "include/cpu/idt.h"
#include "include/cpu/isr.h"
#include "include/cpu/state.h"
#include "include/terminal/terminal.h"
#include "include/common/helper.h"
#include "include/multiboot/multiboot_util.h"

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

void kernel_main(struct kernel_memory_descriptor_t kernel_memory, uint32_t ebx) {

	//initialize isr &irq to help input the 
	// working outputting to serial output . outb(0x3F8,'A');

	multiboot_info_t *mboot_ptr = (multiboot_info_t *) ebx;

	print_multiboot_info( mboot_ptr);

	print_log("\n \nInitializing Descriptor Tables \n \n");
	init_descriptor_tables();

	
	print_log("Initializing Shell \n \n");
	init_shell();

	



	
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
