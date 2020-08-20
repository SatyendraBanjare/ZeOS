global loader                   

extern kernel_main                   

extern kernel_virtual_start
extern kernel_virtual_end
extern kernel_physical_start
extern kernel_physical_end


MAGIC_NUMBER equ 0x1BADB002     

; ALIGN_MODULES   equ 0x00000001 
FLAGS        equ (1<<0 | 1 <<1)  
CHECKSUM     equ -(MAGIC_NUMBER + FLAGS)

global KERNEL_STACK_SIZE
KERNEL_STACK_SIZE equ 0x100000      ; size of stack in bytes (1 MB) this should be less than 4mb page 
                                    ; of kernel


global KERNEL_VIRTUAL_BASE
KERNEL_VIRTUAL_BASE equ 0xC0000000                  ; 3GB
KERNEL_PAGE_NUMBER equ (KERNEL_VIRTUAL_BASE >> 22)  ; Page directory index of kernel's 4MB PTE. = 768

section .data
align 0x1000
global PageDirectoryVirtualAddress
PageDirectoryVirtualAddress:
    dd 0x00000083
    times (KERNEL_PAGE_NUMBER - 1) dd 0                 ; Pages before kernel space.
    ; This page directory entry defines a 4MB page containing the kernel.
    dd 0x00000083
    times (1024 - KERNEL_PAGE_NUMBER - 1) dd 0  ; Pages after the kernel image.

section .text                  
align 4                        

    dd MAGIC_NUMBER             
    dd FLAGS                    ; the flags,
    dd CHECKSUM                 ; and the checksum

loader equ (_loader - KERNEL_VIRTUAL_BASE)
_loader:
    global PageDirectoryPhysicalAddress
    PageDirectoryPhysicalAddress equ (PageDirectoryVirtualAddress - KERNEL_VIRTUAL_BASE) ; 0x104000
    mov ecx, PageDirectoryPhysicalAddress
    mov cr3, ecx                                        ; Load Page Directory Base Register.
 
    mov ecx, cr4
    or ecx, 0x00000010                          ; Set PSE bit in CR4 to enable 4MB pages.
    mov cr4, ecx
 
    mov ecx, cr0
    or ecx, 0x80000000                          ; Set PG bit in CR0 to enable paging.
    mov cr0, ecx
 
    lea ecx, [higher_half_loader]
    jmp ecx

higher_half_loader:
    mov dword [PageDirectoryVirtualAddress], 0
    invlpg [0]

    mov esp, kernel_stack_lowest_address + KERNEL_STACK_SIZE   ; point esp to the start of the
                                                               ; stack (end of memory area)

    add ebx, KERNEL_VIRTUAL_BASE ; make the address virtual
    push ebx 

    push kernel_physical_end
    push kernel_physical_start
    push kernel_virtual_end
    push kernel_virtual_start

    call kernel_main 
    hlt

global kernel_stack_lowest_address
section .bss                        ; Use the 'bss' section for the stack
    align 32                        ; align at 4 bytes for performance reasons
    kernel_stack_lowest_address:    ; label points to beginning of memory
        resb KERNEL_STACK_SIZE      ; reserve stack for the kernel