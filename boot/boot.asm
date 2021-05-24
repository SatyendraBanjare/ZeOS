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

; we set Page write-through, Writable, Present
KERNEL_PT_CFG       equ 00000000000000000000000000001011b
KERNEL_PDT_ID_MAP   equ 00000000000000000000000010001011b

global KERNEL_STACK_SIZE
KERNEL_STACK_SIZE equ 0x100000      ; size of stack in bytes (1 MB) this should be less than 4mb page 
                                    ; of kernel


global KERNEL_VIRTUAL_BASE
KERNEL_VIRTUAL_BASE equ 0xC0000000                  ; 3GB
KERNEL_PAGE_NUMBER equ (KERNEL_VIRTUAL_BASE >> 22)  ; Page directory index of kernel's 4MB PTE. = 768

section .data
align 4
global grub_magic_number
grub_magic_number:
    dd 0
global grub_multiboot_info
grub_multiboot_info:
    dd 0

section .data
align 4096
kernel_pt:
    times 1024 dd 0
kernel_pdt:
    dd KERNEL_PDT_ID_MAP
    times 1023 dd 0

section .text                  
align 4                        

    dd MAGIC_NUMBER             
    dd FLAGS                    ; the flags,
    dd CHECKSUM                 ; and the checksum

loader :
    mov ecx, (grub_magic_number - KERNEL_VIRTUAL_BASE)
    mov [ecx], eax
    mov ecx, (grub_multiboot_info - KERNEL_VIRTUAL_BASE)
    mov [ecx], ebx

    ; set up kernel_pdt to point to kernel_pt
    mov ecx, (kernel_pdt - KERNEL_VIRTUAL_BASE + KERNEL_PAGE_NUMBER*4)
    mov edx, (kernel_pt - KERNEL_VIRTUAL_BASE)
    or  edx, KERNEL_PT_CFG
    mov [ecx], edx

    mov eax, (kernel_pt - KERNEL_VIRTUAL_BASE)
    mov ecx, KERNEL_PT_CFG
.loop:
    mov [eax], ecx
    add eax, 4
    add ecx, 0x1000
    cmp ecx, kernel_physical_end
    jle .loop
    
    mov ecx, (kernel_pdt - KERNEL_VIRTUAL_BASE)
    and ecx, 0xFFFFF000     ; we only care about the upper 20 bits
    or  ecx, 0x08           ; PWT, enable page write through?
    mov cr3, ecx            ; load pdt                                     
 
    mov ecx, cr4
    or ecx, 0x00000010                          ; Set PSE bit in CR4 to enable 4MB pages.
    mov cr4, ecx
 
    mov ecx, cr0
    or ecx, 0x80000000                          ; Set PG bit in CR0 to enable paging.
    mov cr0, ecx
 
    lea ecx, [higher_half_loader]
    jmp ecx

higher_half_loader:
    mov dword [kernel_pdt], 0
    invlpg [0]

    mov esp, kernel_stack_lowest_address + KERNEL_STACK_SIZE   ; point esp to the start of the
                                                               ; stack (end of memory area)

    push kernel_pt
    push kernel_pdt

    add ebx, KERNEL_VIRTUAL_BASE ; make the address virtual
    push ebx 

    push kernel_physical_end
    push kernel_physical_start
    push kernel_virtual_end
    push kernel_virtual_start

    push DWORD [grub_magic_number]
    push DWORD [grub_multiboot_info]


    call kernel_main 
    hlt

global kernel_stack_lowest_address
section .bss                        ; Use the 'bss' section for the stack
    align 4                        ; align at 4 bytes for performance reasons
    kernel_stack_lowest_address:    ; label points to beginning of memory
        resb KERNEL_STACK_SIZE      ; reserve stack for the kernel