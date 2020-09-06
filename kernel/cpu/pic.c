#include "../include/cpu/pic.h"


#define PIC1    0x20    /* IO base address for master PIC */
#define PIC2    0xA0    /* IO base address for slave PIC */
#define PIC1_COMMAND  PIC1
#define PIC1_DATA (PIC1+1)
#define PIC2_COMMAND  PIC2
#define PIC2_DATA (PIC2+1)

#define PIC_EOI   0x20    /* End-of-interrupt command code */

void pic_init() {
  outb(PIC1_DATA, 0b11111101); // enable all interrupts , uncomment to start only keyboard interrupts
  outb(PIC2_DATA, 0b11111111); // Don't enable any interrupts on slave pic (irc 8-15)
  // enable_hardware_interrupts();
  asm volatile("sti");
}

void pic_acknowledge()
{
  outb(PIC1_COMMAND, PIC_EOI);
}