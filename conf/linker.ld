ENTRY(_start)

SECTIONS
{
    . = 0xc0100000;  

    virtual_mem = 0xc0000000;

    kernel_virtual_start = . ;
    
    . -= virtual_mem;

    kernel_physical_start = . ;

    .text ALIGN(0x1000) : 
    {   code = .;
        *(.multiboot)
        *(.text)
    }

    .data ALIGN(0x1000) : 
    {
        *(.data)
        *(.rodata)
    }

    .bss ALIGN(0x1000) :     {
        *(.bss)
    }

    kernel_physical_end = . ;

    . += virtual_mem ;

    kernel_virtual_end = . ;

}