#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>
#include <stddef.h>

#include "util.h"

void serial_init();
void serial_write_byte(uint8_t b);


#endif
