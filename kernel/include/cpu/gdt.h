#ifndef GDT_H
#define GDT_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "state.h"

const uint16_t NULL_SEGMENT_SELECTOR;
const uint16_t KERNAL_CODE_SEGMENT_SELECTOR;
const uint16_t KERNAL_DATA_SEGMENT_SELECTOR;


void init_gdt();


#endif
