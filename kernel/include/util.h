#ifndef UTILH
#define UTILH

/*
Very Basic Helper functions to perform read/write to resgisters.
*/
void outb(uint16_t port, uint8_t value);
void outw(uint16_t port, uint16_t value);

uint8_t inb(uint16_t port);
uint8_t inw(uint16_t port);


#endif