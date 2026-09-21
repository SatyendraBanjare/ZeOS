#include "../include/drivers/keyboard.h"

#define BACKSPACE			0x0E
#define ENTER     			0x1C
#define DELETE              0x53 
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
int buff_pointer = 0;   /* cursor position inside key_buffer */
int max_length = 0;     /* number of characters in key_buffer */
int ctrl_press=0;

/* Print key_buffer[buff_pointer..max_length) followed by `blanks` spaces, then
 * put the screen cursor back at buff_pointer. */
static void redraw_tail(int blanks){
    char str[2] = {'\0', '\0'};
    for(int i = buff_pointer; i < max_length; i++){
        str[0] = key_buffer[i];
        zprint(str);
    }
    for(int i = 0; i < blanks; i++){
        zprint(" ");
    }
    for(int i = 0; i < (max_length - buff_pointer) + blanks; i++){
        zprint_left();
    }
}

static void insert_char(char c){
    if(max_length >= 99) return;
    for(int i = max_length; i > buff_pointer; i--){
        key_buffer[i] = key_buffer[i - 1];
    }
    key_buffer[buff_pointer] = c;
    max_length++;
    buff_pointer++;
    key_buffer[max_length] = '\0';

    char str[2] = {c, '\0'};
    zprint(str);
    redraw_tail(0);
}

static void delete_before_cursor(){
    if(buff_pointer == 0) return;
    for(int i = buff_pointer - 1; i < max_length - 1; i++){
        key_buffer[i] = key_buffer[i + 1];
    }
    max_length--;
    buff_pointer--;
    key_buffer[max_length] = '\0';

    zprint_left();
    redraw_tail(1);
}

void manage_delete(){
    if(buff_pointer >= max_length) return;
    for(int i = buff_pointer; i < max_length - 1; i++){
        key_buffer[i] = key_buffer[i + 1];
    }
    max_length--;
    key_buffer[max_length] = '\0';

    redraw_tail(1);
}

void manage_left(){
    if(buff_pointer > 0){
        zprint_left();
        buff_pointer--;
    }
}

void manage_right(){
    if(buff_pointer < max_length){
        zprint_right();
        buff_pointer++;
    }
}

void start_of_line(){
    while(buff_pointer > 0) manage_left();
}

void end_of_line(){
    while(buff_pointer < max_length) manage_right();
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

static int extended_key = 0;   /* previous byte was the 0xE0 prefix */

/* Second byte of an extended (0xE0-prefixed) key press */
static void handle_extended_key(int code){
    if (code == ARROW_UP && max_length == 0){
        zprint(kb_c);
        strcpy_i(kb_c);
        buff_pointer = strlen(kb_c);
        max_length = buff_pointer;
    } else if (code == ARROW_LEFT){
        manage_left();
    } else if (code == ARROW_RIGHT){
        manage_right();
    } else if (code == ARROW_DOWN){
        zprint("DOWN");
    } else if (code == DELETE){
        manage_delete();
    }
}

void keyboard_callback() {
    /* The PIC leaves us one scancode byte in port 0x60; read it exactly once */
    int temp_scancode = inb(0x60);

    /* Extended keys arrive as 0xE0 followed by the key code (release: code|0x80) */
    if (temp_scancode == 0xE0){
        extended_key = 1;
        return;
    }
    if (extended_key){
        extended_key = 0;
        if (!(temp_scancode & 0x80)) handle_extended_key(temp_scancode);
        return;
    }

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
        if(temp_scancode == 0x1d)
           { ctrl_press=0;}
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
        delete_before_cursor();
    } else if (temp_scancode == ENTER) {
        end_of_line();
        zprint("\n");
        strcpy_h(key_buffer);
        user_input(key_buffer); /* kernel-controlled function */
        
        flush_buffer(key_buffer);
        buff_pointer =0;
        max_length =0;
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
            clear_screen(get_alias());
            flush_buffer(key_buffer);
            buff_pointer =0;
            max_length =0;
            ctrl_press =0;
        }else if (ctrl_press == 1 && (letter == 'A' || letter == 'a'))
        {
            start_of_line();
            ctrl_press =0;
        }else if (ctrl_press == 1 && (letter == 'E' || letter == 'e'))
        {
            end_of_line();
            ctrl_press =0;
        }
        else{
        if (letter != '\0') insert_char(letter);
    }
    }
    }

    // (void)(state);

    // void *(state);
}

// void init_keyboard() {
//    register_interrupt_handler(IRQ1, keyboard_callback); 
// }
