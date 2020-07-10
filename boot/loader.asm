%include "boot/include/gdt.asm"

BITS 32
 
global load_kernel
load_kernel:
    
    extern kernel_main
    call kernel_main
    jmp $