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

/* Top bar: "CURRENT DIRECTORY : <name>" padded to the "|  Up Time:" column */
void print_footer(){
	char line[80];
	const char *label = "CURRENT DIRECTORY : ";
	const char *tail = "|  Up Time:  ";
	const int name_col = 20;     // strlen(label)
	const int tail_col = 54;     // column where the '|' separator starts
	int pos = 0;

	// Directory name = last component of the current path ("/" for root)
	const char *path = vfs_get_current_directory();
	const char *name = "/";
	int name_len = 1;
	if (path && path[0]) {
		int end = strlen((char*)path);
		while (end > 1 && path[end - 1] == '/') end--;
		int start = end;
		while (start > 0 && path[start - 1] != '/') start--;
		if (end > start) {
			name = path + start;
			name_len = end - start;
		}
	}
	if (name_len > tail_col - name_col - 1) name_len = tail_col - name_col - 1;

	for (int i = 0; label[i]; i++) line[pos++] = label[i];
	for (int i = 0; i < name_len; i++) line[pos++] = name[i];
	while (pos < tail_col) line[pos++] = ' ';
	for (int i = 0; tail[i]; i++) line[pos++] = tail[i];
	line[pos] = '\0';

	zprint_header(line);
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
    
    print_footer();
    clear_screen(get_alias());
}

static int prompt_printed = 0;   // set when a command already drew the prompt (clear)

void manage_input(char *input){
    // Trim whitespace from input
    strip_extra_spaces(input);

    print_log("RECEIVED INPUT");
    print_log(input);

    if (strcmp(input, "end") == 0) {
        zprint("Stopping the CPU. Bye!\n");
        asm volatile("hlt");
    } else if (strcmp (input, "clear") == 0){
        clear_screen(get_alias());
        prompt_printed = 1;
    } else if (strcmp (input, "help") == 0){
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
    
    if (!prompt_printed) zprint("\n");
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



/*
 * echo <text> [> file]
 * Parsed from the original (case-preserved) line. Double quotes group words and
 * are removed from the output; a '>' inside quotes is literal text.
 */
static void handle_echo(const char *raw) {
    char text[100];
    char file[MAX_FILENAME_LENGTH];
    int t = 0, f = 0;
    int in_quote = 0, redirect = 0, pending_space = 0;
    int i = 4; // skip "echo"

    while (raw[i] == ' ') i++;

    for (; raw[i]; i++) {
        char c = raw[i];
        if (c == '"') {
            in_quote = !in_quote;
            if (redirect == 0 && pending_space && t > 0) { text[t++] = ' '; }
            pending_space = 0;
            continue;
        }
        if (!in_quote && c == '>' && !redirect) {
            redirect = 1;
            pending_space = 0;
            continue;
        }
        if (!in_quote && c == ' ') {
            pending_space = 1;
            continue;
        }
        if (redirect) {
            if (f < MAX_FILENAME_LENGTH - 1) {
                file[f++] = (c >= 'A' && c <= 'Z') ? c + 32 : c;
            }
        } else {
            if (pending_space && t > 0 && t < 99) { text[t++] = ' '; }
            pending_space = 0;
            if (t < 99) text[t++] = c;
        }
    }
    text[t] = '\0';
    file[f] = '\0';

    if (in_quote) {
        zprint("echo: unterminated quote\n");
    } else if (redirect) {
        if (f == 0) {
            zprint("echo: missing file name\n");
        } else {
            vfs_shell_echo(text, file);
        }
    } else {
        zprint(text);
        zprint("\n");
    }
}

void user_input(char *input) {
    int n; //number of words
    int i; //loop counter 
    char arr[20][20];
    char raw[100];

    // Keep the original line: echo needs its case and quoted spacing intact
    for (i = 0; i < 99 && input[i]; i++) raw[i] = input[i];
    raw[i] = '\0';

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
        print_footer(); // refresh the directory name in the top bar
    } else if (n >= 0 && strcmp(arr[0], "pwd") == 0) {
        vfs_shell_pwd();
    } else if (n >= 1 && strcmp(arr[0], "cat") == 0) {
        if (strcmp(arr[1], "\0") != 0) {
            vfs_shell_cat(arr[1]);
        } else {
            zprint("cat: missing file name\n");
        }
    } else if (strcmp(arr[0], "echo") == 0) {
        handle_echo(raw);
    } else {
        // Handle other commands or single word input
        manage_input(input);
    }

    // Print new prompt
    if (!prompt_printed) {
        zprint_new_line(get_alias());
        zprint_new_line("> ");
    }
    prompt_printed = 0;
}


