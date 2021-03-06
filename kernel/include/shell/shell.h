#ifndef SHELL_H
#define SHELL_H

#include <stdint.h>
#include <stddef.h>
#include "../terminal/terminal.h"
#include "../common/helper.h"
#include "../cpu/timer.h"

// int has_access();
// void run_shell();
// void clear_buff();
// void clear_history();
// void clear_command_list();

void user_input(char *buff);
void init_shell();

char * get_alias();
int get_uptime();
void print_footer();
void print_header();
void print_blank();
#endif
