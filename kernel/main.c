#include "terminal/terminal.h"

#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

void kernel_main(void) {
    terminal_initialize();
    terminal_writestring("Finally Booted successfully !!\n");
    terminal_writestring("Hello, world!!!\n");
}
