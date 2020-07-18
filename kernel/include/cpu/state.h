#ifndef CPU_STATE_H
#define CPU_STATE_H

typedef struct {
   uint32_t ds; /* Data segment selector */
   uint32_t edi, esi, ebp, useless, ebx, edx, ecx, eax; /* Pushed by pusha. */
   uint32_t int_no, err_code; /* Interrupt number and error code (if applicable) */
   uint32_t eip, cs, eflags, esp, ss; /* Pushed by the processor automatically */
} cpu_state;


#endif