#ifndef MULTIBOOT_UTIL
#define MULTIBOOT_UTIL

#include "multiboot.h"
#include "../common/helper.h"
#include "../terminal/terminal.h"

void print_multiboot_info(struct multiboot_info *mboot_ptr);
uint32_t p_to_v(uint32_t physical_address);

#endif