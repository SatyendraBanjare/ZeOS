#ifndef CPU_STATE_H
#define CPU_STATE_H

/*
* This describes the Registers at any given point of
* time thus describing the cpu state completely.
*/
typedef struct {
   uint32_t ds; /* Data segment selector */
   uint32_t edi, esi, ebp, useless, ebx, edx, ecx, eax; /* Pushed by pusha. */
   uint32_t int_no, err_code; /* Interrupt number and error code (if applicable) */
   uint32_t eip, cs, eflags, esp, ss; /* Pushed by the processor automatically */
} cpu_state;

struct kernel_memory_descriptor_t {
  uint32_t kernel_virtual_start;
  uint32_t kernel_virtual_end;
  uint32_t kernel_physical_start;
  uint32_t kernel_physical_end;
};

uint32_t KERNEL_VIRTUAL_BASE;

void * PageDirectoryVirtualAddress;
void * PageDirectoryPhysicalAddress;

#endif