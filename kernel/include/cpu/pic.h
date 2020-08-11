#ifndef PIC_H
#define PIC_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "state.h"
#include "util.h"


void pic_init();
void pic_acknowledge();


#endif
