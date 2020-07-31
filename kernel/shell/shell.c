/*
manages the Header and footer.

header : welcome message, username

footer : current work directory , arg counts

*/

#include "../include/shell/shell.h"

void manage_input(char *input){
    if (strcmp(input, "END") == 0) {
        zprint("Stopping the CPU. Bye!\n");
        asm volatile("hlt");
    } else if (strcmp (input, "CLEAR") == 0){
        clear_screen();
    } else if (strcmp (input, "HELP") == 0){
        zprint(input);
    } else if (strcmp (input, "delay") == 0){
    
    uint32_t time_old = rdtsc() +5;
    uint32_t tn = rdtsc();
    while((tn - time_old) != 0)
    {
        tn = rdtsc();
        // zprint("HI \n");
    }if (tn - time_old == 0)
    {
        zprint("DONE !!");
    }

    }

    zprint("You said: ");
    zprint(input);
    zprint("\n> ");
}


int is_space(char c){
    if( c == ' '){return 1;}
    else{return 0;}
}

void strip_extra_spaces(char* str) {
  int i, x;
  for(i=x=0; str[i]; ++i)
    if(!is_space(str[i]) || (i > 0 && !is_space(str[i-1])))
      str[x++] = str[i];
  str[x] = '\0';
}



int getWords(char *base, char target[20][20])
{
    int n=0,i,j=0;
    
    for(i=0;1;i++)
    {
        if(base[i]!=' '){
            target[n][j++]=base[i];
        }
        else{
            target[n][j++]='\0';//insert NULL
            n++;
            j=0;
        }
        if(base[i]=='\0')
            break;
    }
    return n;
    
}



void user_input(char *input) {

    int n; //number of words
    int i; //loop counter 
    // char str[]="This is Mike";
    char arr[20][20];
    
    strip_extra_spaces(input);

    n=getWords(input,arr);
    
    for(i=0;i<=n;i++){
        if (strcmp(arr[i],"\0") != 0 )
        {
        zprint(arr[i]);
        zprint("\n");
        }
        
    }

    manage_input(input);    
}
