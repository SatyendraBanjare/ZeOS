#include "../include/drivers/keyboard.h"

#define BACKSPACE			0x0E
#define ENTER     			0x1C
#define ARROW_UP			0x48
#define	ARROW_DOWN			0x50
#define	ARROW_LEFT			0x4B
#define ARROW_RIGHT			0x4D

static char key_buffer[100] ;
static char kb_c[100];

#define SC_MAX 57
const char *sc_name[] = { "ERROR", "Esc", "1", "2", "3", "4", "5", "6", 
    "7", "8", "9", "0", "-", "=", "Backspace", "Tab", "Q", "W", "E", 
        "R", "T", "Y", "U", "I", "O", "P", "[", "]", "Enter", "Lctrl", 
        "A", "S", "D", "F", "G", "H", "J", "K", "L", ";", "'", "`", 
        "LShift", "\\", "Z", "X", "C", "V", "B", "N", "M", ",", ".", 
        "/", "RShift", "Keypad *", "LAlt", "Spacebar"};

const char sc_ascii_non_shift[] ={ '\0', '\0', '1', '2', '3', '4', '5', '6',     
    '7', '8', '9', '0', '-', '=', '\0', '\t', 'q', 'w', 'e', 'r', 't', 'y', 
        'u', 'i', 'o', 'p', '[', ']', '\0', '\0', 'a', 's', 'd', 'f', 'g', 
        'h', 'j', 'k', 'l', ';', '\'', '`', '\0', '\\', 'z', 'x', 'c', 'v', 
        'b', 'n', 'm', ',', '.', '/', '\0', '\0', '\0', ' '};

const char sc_ascii_shift[] = { '\0', '\0', '!', '@', '#', '$', '%', '^',     
    '&', '*', '(', ')', '_', '+', '\0', '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 
        'U', 'I', 'O', 'P', '[', ']', '\0', '\0', 'A', 'S', 'D', 'F', 'G', 
        'H', 'J', 'K', 'L', ':', '\"', '~', '\0', '|', 'Z', 'X', 'C', 'V', 
        'B', 'N', 'M', '<', '>', '?', '\0', '\0', '\0', ' '};

int shift_down = 0;
int buff_pointer = 0;
int max_back =0;
int ctrl_press=0;


void manage_left(){
    if(max_back >0 && buff_pointer >0){
        zprint_left();
    buff_pointer --;
    max_back--;
    }
    else{
        return;
    }
}

void manage_right(){
    zprint_right();
    buff_pointer ++;
}

void flush_buffer(char kb[100]){
    for(int i=0;i<100;i++){
        kb[i]= '\0';
    }
}

void strcpy_h(char kb[100]){

    for (int i = 0; i < 100; ++i)
    {
        kb_c[i] = kb[i];
    }

}

void strcpy_i(char kb[100]){

    for (int i = 0; i < 100; ++i)
    {
        key_buffer[i] = kb[i];
    }

}

static void keyboard_callback(cpu_state *state) {
    /* The PIC leaves us the scancode in port 0x60 */
    int scancode = inb(0x60);
    int temp_scancode = inb(0x60);
    // int temp_scancode_2 = inb(0x60);

    // int sss = temp_scancode_2;
    //managing control key
    // if((sss&0x80)){
    //      sss &= 0x7F;
    //     if(sss == 0x1d )
    //        { zprint("cRR"); ctrl_press=!ctrl_press;}
    //     return;
    // }

    // Control key
    if(temp_scancode == 0x1d )
    {
        ctrl_press =1;
    }

    // check if shift is released 
    if((temp_scancode&0x80)){

         temp_scancode &= 0x7F;
        if(temp_scancode == 0x2a || temp_scancode == 0x36)
           { shift_down=0;}
        return;
    }
    

    if(temp_scancode == 0x2a || temp_scancode == 0x36)
    { //is_shift_pressed = 1;
               shift_down=1;

    }

    //Caps Lock
    if(temp_scancode == 0x3a){
        shift_down = !shift_down;
    }

    if (temp_scancode <= SC_MAX) {
    if (temp_scancode == BACKSPACE) {
        if(max_back >0){
            max_back--;
            buff_pointer--;
            key_buffer[buff_pointer]='\0';
        
        zprint_backspace();
    }
    } else if (temp_scancode == ENTER) {
        zprint("\n");
        // zprint(key_buffer);
        strcpy_h(key_buffer);
        user_input(key_buffer); /* kernel-controlled function */
        
        flush_buffer(key_buffer);
        buff_pointer    =0;
        max_back=0;    
    } else {

        char letter;

        // zprint(int_to_str(scancode));
        if(shift_down == 0){
         letter = sc_ascii_non_shift[(int)temp_scancode];}
        else {
            letter = sc_ascii_shift[(int)temp_scancode];
        }
        if (ctrl_press == 1 && (letter == 'l' || letter == 'L'))
        {
            clear_screen();
            flush_buffer(key_buffer);
            buff_pointer =0;
            max_back=0;
            ctrl_press =0;
        }else{
        /* Remember that zprint only accepts char[] */
        char str[2] = {letter, '\0'};
        // append(key_buffer, letter);
        key_buffer[buff_pointer] = letter;
        buff_pointer++;
        max_back++;
        zprint(str);
    }
    }
    }

    if (scancode == 0xE0){
            uint8_t scan_code_2 = inb(0x60);
            if (scan_code_2 == ARROW_UP) if(max_back == 0){
                zprint(kb_c);
                strcpy_i(kb_c);
                buff_pointer = strlen(kb_c);
                }
            if (scan_code_2 == ARROW_LEFT)
            {manage_left();}
            if (scan_code_2 == ARROW_RIGHT)
            {manage_right();}
            if (scan_code_2 == ARROW_DOWN)
            {zprint("DOWN");}
    }



    // void *(state);
}

void init_keyboard() {
   register_interrupt_handler(IRQ1, keyboard_callback); 
}
