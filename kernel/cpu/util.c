#include "../include/cpu/util.h"

/*
* Write to a port.
*/
void outb(uint16_t port, uint8_t value)
{
    asm volatile("out %%al, %%dx" : : "a" (value), "d" (port));
}

void outw(uint16_t port, uint16_t value)
{
    /*be careful here to use %%ax (16-bit) register */
    asm volatile("out %%ax, %%dx" : : "a" (value), "d" (port));
}

/*
* read from a port.
*/
uint8_t inb(uint16_t port)
{
    uint8_t result;
    asm("in %%dx, %%al" : "=a" (result) : "d" (port));
    return result;
}

uint16_t inw(uint16_t port)
{
    uint16_t result;

    /*be careful here to use %%ax (16-bit) register */
    asm("in %%dx, %%ax" : "=a" (result) : "d" (port));
    return result;
}
