%include "boot/include/gdt.asm"

BITS 32

[GLOBAL load_kernel]

load_kernel:
    
    extern kernel_main
    call kernel_main
    jmp $