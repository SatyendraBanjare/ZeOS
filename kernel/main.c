#include "include/multiboot.h"
#include "include/cpu/gdt.h"
#include "include/cpu/idt.h"
#include "include/cpu/isr.h"
#include "include/terminal/terminal.h"
#include "include/common/helper.h"

#include "include/fs/initrd.h"
#include "include/fs/fs.h"
#include "include/cpu/memory/paging.h"
#include "include/cpu/memory/heap.h"

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

uint32_t placement_address;

void kernel_main(struct multiboot_info *mboot_ptr) {

	//initialize isr &irq to help input the 
	init_descriptor_tables();

	init_shell();
	if (mboot_ptr->mods_count == 0)
	{
		zprint("JJJJ");
	}

	// uint32_t initrd_location = *((uint32_t*)mboot_ptr->mods_addr);

	// zprint(int_to_str(initrd_location));

	// User Acces Stuff
	// while(shell_access_flag == 0){
	// 	if(has_access() == 1)
	// 	{
	// 		shell_access_flag = 1;
	// 		start_execution();
	// 	}
	// }

	// start_execution();


    // Find the location of our initial ramdisk.
    // ASSERT(mboot_ptr->mods_count > 0);
    uint32_t initrd_location = *((uint32_t*)mboot_ptr->mods_addr);
    
    uint32_t initrd_end = *(uint32_t*)(mboot_ptr->mods_addr+4);
    // Don't trample our module with placement accesses, please!
    placement_address = initrd_end;

    // Start paging.
    initialise_paging();

    // Initialise the initial ramdisk, and set it as the filesystem root.
    fs_root = initialise_initrd(initrd_location);

    // list the contents of /
    int i = 0;
    struct dirent *node = 0;
    while ( (node = readdir_fs(fs_root, i)) != 0)
    {
        zprint("Found file ");
        zprint(node->name);
        fs_node_t *fsnode = finddir_fs(fs_root, node->name);

        if ((fsnode->flags&0x7) == FS_DIRECTORY)
        {
            zprint("\n\t(directory)\n");
        }
        else
        {
            zprint("\n\t contents: \"");
            // char buf[256];
            // uint32_t sz = read_fs(fsnode, 0, 256, buf);
            // int j;
            // for (j = 0; j < sz; j++)
            //     monitor_put(buf[j]);
            
            // zprint("\"\n");
        }
        i++;
    }

}
