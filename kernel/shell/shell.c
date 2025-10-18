/*
manages the Header and footer.

header : welcome message, username

footer : current work directory , arg counts

*/

#include "../include/shell/shell.h"
#include "../include/fs/fs.h"

char * alias = "Zeus";
// int uptime;
// char curr_dir_name[33] ;


char * get_alias(){
	return alias;
}

// char * get_curr_dir_name(){
// 	return curr_dir_name;
// }

// int get_uptime(){
// 	return uptime;
// }

void print_header(){
	zprint_footer("                               ZEOS Operating System !!                               ");
}

void print_footer(){
	zprint_header("CURRENT DIRECTORY : /DEV/                             |  Up Time:  ");
}

void print_welcome_message(){
    zprint(

    "                                                                                "
    "                                                                                "
    "                                                                                "
    "                               WELCOME TO ZEOS !!                               "
    "                                                                                "
    "                                                                                "
    "                                                                                "
    "                                                                                "                                                 
    "                                                                                " 
    "          ****           *      ***** **          * ***          *******        "
    "         *  *************    ******  **** *     *  ****        *       ***      " 
    "        *     **********    **   *  * ****     *  *  ***      *         **      "
    "        *             *    *    *  *   **     *  **   ***     **        *       "
    "         **          *         *  *          *  ***    ***     ***              "
    "                    *         ** **         **   **     **    ** ***            "
    "                   *          ** **         **   **     **     *** ***          "
    "                  *           ** ******     **   **     **       *** ***        "
    "                 *            ** *****      **   **     **         *** ***      "
    "                *             ** **         **   **     **           ** ***     "
    "               *              *  **          **  **     **            ** **     "
    "              *                  *            ** *      *              * *      "
    "          ****           *   ****         *    ***     *     ***        *       "
    "         *  *************   *  ***********      *******     *  *********        "
    "        *     **********   *     ******           ***      *     *****          "
    "                           *                               *                    "
    "                            **                              **                  "
    "                                                                                "
    "                                                                                "
    "                                                                                "
    "                                                                                "
    );
}

void print_blank(){zprint("\n  \n \n \n \n \n \n \n \n \n \n \n \n \n \n \n \n \
 \n \n \n \n \n \n \n \n \n \n \n \n \n \n \n \n \n \n \n \n \n \n \n \n \n \n  \
 \n \n \n \n \n \n \n \n \n \n ");
}

void delay2(){
    uint32_t time_old = rdtsc() + 2;
    uint32_t tn = rdtsc();
    while((tn - time_old) != 0)
    {
        tn = rdtsc();
        // zprint("HI \n");
    }
}

void init_shell(){
    
    // uptime = rdtsc();
    
    clear_screen_full();
    print_welcome_message();
    delay2();
    clear_screen_full();
    print_header();
    print_footer();
    print_blank();
    
    // Initialize VFS
    int vfs_result = vfs_init();
    if (vfs_result != VFS_SUCCESS) {
        zprint("Error: Failed to initialize virtual file system\n");
    } else {
        zprint("Virtual File System initialized successfully\n");
        
        // Create some default directories
        vfs_create_directory("/home", PERM_READ | PERM_WRITE | PERM_EXECUTE);
        vfs_create_directory("/tmp", PERM_READ | PERM_WRITE | PERM_EXECUTE);
        vfs_create_directory("/usr", PERM_READ | PERM_WRITE | PERM_EXECUTE);
        vfs_create_directory("/etc", PERM_READ | PERM_WRITE | PERM_EXECUTE);
        
        // Create a sample file
        vfs_create_file("/readme.txt", PERM_READ | PERM_WRITE);
        int fd = vfs_open("/readme.txt", O_WRONLY | O_TRUNC);
        if (fd >= 0) {
            const char* welcome_text = "Welcome to ZeOS Virtual File System!\n\nThis is a sample file to demonstrate the VFS capabilities.\nYou can use commands like ls, cat, mkdir, touch, rm, cd, and pwd.\n\nTry: ls / to see the directory structure.";
            vfs_write(fd, welcome_text, strlen((char*)welcome_text));
            vfs_close(fd);
        }
        
        zprint("Sample directories and files created\n");
    }
    
    clear_screen(get_alias());
}

void manage_input(char *input){
    // Trim whitespace from input
    strip_extra_spaces(input);

    print_log("RECEIVED INPUT");
    print_log(input);

    if (strcmp(input, "END") == 0) {
        zprint("Stopping the CPU. Bye!\n");
        asm volatile("hlt");
    } else if (strcmp (input, "CLEAR") == 0){
        clear_screen();
    } else if (strcmp (input, "HELP") == 0){
        zprint("Available commands:\n");
        zprint("  ls [path]       - list directory contents\n");
        zprint("  mkdir <path>    - create directory\n");
        zprint("  touch <file>    - create file\n");
        zprint("  rm <path>       - remove file or directory\n");
        zprint("  cd <path>       - change directory\n");
        zprint("  pwd             - print working directory\n");
        zprint("  cat <file>      - display file contents\n");
        zprint("  echo <text>     - display text (use > file to redirect)\n");
        zprint("  alias           - change user alias\n");
        zprint("  time            - show system time\n");
        zprint("  delay           - test delay function\n");
        zprint("  clear           - clear screen\n");
        zprint("  end             - halt system\n");
    } else if (strcmp (input, "alias") == 0){
        alias = "dedd";
    } 
    else if (strcmp (input, "delay") == 0){
    
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
    else if(strcmp(input, "time") == 0){
        zprint_int(rdtsc());
    } else {
        // Unknown command - show help
        zprint("Unknown command. Type 'help' for available commands.\n");
    }
    
    zprint("\n");	
    zprint_new_line(get_alias());
    zprint_new_line("> ");
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
    char arr[20][20];
    
    strip_extra_spaces(input);
    
    // Convert input to lowercase for case-insensitive commands
    for (i = 0; input[i]; i++) {
        if (input[i] >= 'A' && input[i] <= 'Z') {
            input[i] = input[i] + 32;
        }
    }
    
    n=getWords(input,arr);
    
    // Handle VFS commands
    if (n >= 0 && strcmp(arr[0], "ls") == 0) {
        if (n >= 1 && strcmp(arr[1], "\0") != 0) {
            vfs_shell_ls(arr[1]);
        } else {
            vfs_shell_ls(NULL); // List current directory
        }
    } else if (n >= 1 && strcmp(arr[0], "mkdir") == 0) {
        if (strcmp(arr[1], "\0") != 0) {
            vfs_shell_mkdir(arr[1]);
        } else {
            zprint("mkdir: missing directory name\n");
        }
    } else if (n >= 1 && strcmp(arr[0], "touch") == 0) {
        if (strcmp(arr[1], "\0") != 0) {
            vfs_shell_touch(arr[1]);
        } else {
            zprint("touch: missing file name\n");
        }
    } else if (n >= 1 && strcmp(arr[0], "rm") == 0) {
        if (strcmp(arr[1], "\0") != 0) {
            vfs_shell_rm(arr[1]);
        } else {
            zprint("rm: missing file name\n");
        }
    } else if (n >= 0 && strcmp(arr[0], "cd") == 0) {
        if (n >= 1 && strcmp(arr[1], "\0") != 0) {
            vfs_shell_cd(arr[1]);
        } else {
            vfs_shell_cd("/"); // Go to root if no path
        }
    } else if (n >= 0 && strcmp(arr[0], "pwd") == 0) {
        vfs_shell_pwd();
    } else if (n >= 1 && strcmp(arr[0], "cat") == 0) {
        if (strcmp(arr[1], "\0") != 0) {
            vfs_shell_cat(arr[1]);
        } else {
            zprint("cat: missing file name\n");
        }
    } else if (n >= 1 && strcmp(arr[0], "echo") == 0) {
        // Handle echo with potential redirection
        if (n >= 3 && strcmp(arr[2], ">") == 0 && strcmp(arr[3], "\0") != 0) {
            // Echo to file: echo text > filename
            vfs_shell_echo(arr[1], arr[3]);
        } else if (strcmp(arr[1], "\0") != 0) {
            // Just echo to screen
            zprint(arr[1]);
            zprint("\n");
        } else {
            zprint("echo: missing text\n");
        }
    } else {
        // Handle other commands or single word input
        manage_input(input);
    }

    // Print new prompt
    zprint_new_line(get_alias());
    zprint_new_line("> ");
}


