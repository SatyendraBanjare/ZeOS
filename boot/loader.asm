%include "boot/include/gdt.asm"
%include "boot/include/idt.asm"
%include "boot/include/interrupt.asm"
%include "boot/include/tss.asm"

BITS 32

[GLOBAL load_kernel]

load_kernel:
    
    extern kernel_main
    call kernel_main
    jmp $