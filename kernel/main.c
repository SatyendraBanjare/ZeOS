#include "include/multiboot.h"
#include "include/cpu/gdt.h"
#include "include/cpu/idt.h"
#include "include/cpu/isr.h"
#include "include/terminal/terminal.h"

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

void start_execution(){
	zprint("HI");

	 asm("int $2");
    asm("int $3");
}

void kernel_main(void) {

	//initialize isr &irq to help input the 
	init_descriptor_tables();

	// User Acces Stuff
	// while(shell_access_flag == 0){
	// 	if(has_access() == 1)
	// 	{
	// 		shell_access_flag = 1;
	// 		start_execution();
	// 	}
	// }

	start_execution();


}
