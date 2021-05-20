#include "../include/common/helper.h"
#include "../include/cpu/isr.h"
#include "../include/cpu/util.h"
#include "../include/cpu/pic.h"
#include "../include/cpu/timer.h"
#include "../include/terminal/terminal.h"
#include "../include/drivers/keyboard.h"

#define INT_TIMER 0x00000008
#define INT_KEYBOARD 0x00000009
#define INT_GENERAL_PROTECTION_FAULT 0x0000000D
#define INT_PAGE_FAULT 0x0000000E

void interrupt_handler(struct cpu_state cpu) {
  print_log("\n!!! Interrupt\n");
  print_log("interrupt_number: ");
  print_log_int( cpu.int_no,10);
  print_log("\n");

  print_log("cpu.err_code: ");
  print_log_int( cpu.err_code,10);
  print_log("\n");

  // print_log("\nStack trace:\n");
  // print_log_int( eip);
  // print_log(" : ");
  // char * symbol_name = address_to_symbol_name(eip);
  // fprintf(LOG, symbol_name);
  // print_log("\n");
  // uint32_t ebp = cpu.ebp;
  // while (ebp & 0xC0100000) {
  //   uint32_t eip = ((uint32_t*) ebp)[1];
  //   print_log_int( eip);
  //   print_log(" : ");
  //   char * symbol_name = address_to_symbol_name(eip);
  //   fprintf(LOG, symbol_name);
  //   print_log("\n");

  //   ebp = *((uint32_t*)ebp);
  // }
  print_log("\n");

  switch(cpu.int_no) {

    case(32):{// Timer interrupt_handler();
        print_log("TIMER");
        timer_callback();
          break;
    }

    case(33):{// keyboard_interrupt_handler();
        print_log("KEYBOARD");
        keyboard_callback();
          break;
    }
    case(INT_PAGE_FAULT):{  
          print_log("Interrupt was a page fault. Here's what I know:\n");
          print_log("- Tried to access linear address ");
          print_log_int( cpu.cr2,10);
          print_log("\n ");
          print_log_int( cpu.err_code,10);
          print_log("\n");
          if (cpu.err_code & 0x1) {
            print_log("- Couldn't complete because of page-protection violation\n");
          } else {
            print_log("- Couldn't complete because page was not present\n");
          }
          if (cpu.err_code & 0x2) {
            print_log("- This was an attempt to WRITE to this address.\n");
          } else {
            print_log("- This was an attempt to READ from this address.\n");
          }
          if (cpu.err_code & 0x4) {
            print_log("- Memory access came from user.\n");
          } else {
            print_log("- Memory access came from kernel.\n");
          }
          if (cpu.err_code & 0x08) {
            print_log("- caused by reading a 1 in a reserved field.\n");
          }
          if (cpu.err_code & 0x10) {
            print_log("- caused by an instruction fetch.\n");
          }

          // Cause Triple Fault !
          while(1){};
          break;
    }
    default:
      print_log("ERROR: Unabled to handle interrupt: ");
      print_log_int( cpu.int_no,10);
      print_log("\n");
      break;
  }

  pic_acknowledge();
}
