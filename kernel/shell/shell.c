/*
manages the Header and footer.

header : welcome message, username

footer : current work directory , arg counts

*/

#include "../include/shell/shell.h"
#include "../include/fs/fs.h"
#include "../include/cpu/thread.h"
#include "../include/fs/initrd.h"

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

/* "  label : <value> bytes (<kb> KB)" */
static void print_mem_line(char *label, uint32_t bytes){
    char num[16];
    zprint(label);
    int_to_ascii((int)bytes, num);
    zprint(num);
    zprint(" bytes (");
    int_to_ascii((int)(bytes / 1024), num);
    zprint(num);
    zprint(" KB)\n");
}

static void print_mem_count(char *label, uint32_t n){
    char num[16];
    zprint(label);
    int_to_ascii((int)n, num);
    zprint(num);
    zprint("\n");
}

/* mem - show kernel heap and stack usage */
static void show_memory_usage(){
    heap_stats_t st;
    heap_get_stats(&st);

    uint32_t stack_base, stack_size;
    thread_stack_bounds(&stack_base, &stack_size);
    uint32_t stack_top = stack_base + stack_size;
    uint32_t esp = (uint32_t)(uintptr_t)get_current_stack_pointer();

    zprint("Kernel heap:\n");
    print_mem_line("  in use        : ", st.used_bytes);
    print_mem_line("  free (reuse)  : ", st.free_bytes);
    print_mem_line("  block headers : ", st.overhead_bytes);
    print_mem_line("  heap size     : ", st.heap_end - st.heap_start);
    print_mem_line("  can still grow: ", st.heap_limit > st.heap_end ? st.heap_limit - st.heap_end : 0);
    print_mem_line("  largest free  : ", st.largest_free);
    print_mem_count("  blocks used   : ", st.used_blocks);
    print_mem_count("  blocks free   : ", st.free_blocks);
    zprint("Stack (running thread):\n");
    print_mem_line("  in use        : ", (esp >= stack_base && esp < stack_top) ? stack_top - esp : 0);
    print_mem_line("  total         : ", stack_size);
}

/* ---- kernel threads: demo bodies and shell commands ---- */

// Spins forever; shows that the scheduler preempts a thread that never yields
static void demo_counter(void *arg){
    (void)arg;
    thread_t *me = thread_current();
    while (1) *(volatile uint32_t *)&me->work += 1;
}

// Does a little work, then sleeps half a second
static void demo_ticker(void *arg){
    (void)arg;
    thread_t *me = thread_current();
    while (1) {
        *(volatile uint32_t *)&me->work += 1;
        thread_sleep(25);
    }
}

// Finite job: exits on its own and gets reaped
static void demo_burst(void *arg){
    (void)arg;
    thread_t *me = thread_current();
    for (uint32_t i = 0; i < 3000000; i++) *(volatile uint32_t *)&me->work += 1;
}

static void print_padded(char *text, int width){
    int len = strlen(text);
    zprint(text);
    for (int i = len; i < width; i++) zprint(" ");
}

static void print_padded_int(uint32_t n, int width){
    char num[16];
    int_to_ascii((int)n, num);
    print_padded(num, width);
}

/* ps - list kernel threads */
static void show_threads(){
    zprint("ID  NAME        STATE     TICKS   WORK\n");
    for (thread_t *t = thread_first(); t; t = t->next) {
        print_padded_int(t->id, 4);
        print_padded(t->name, 12);
        print_padded((char*)thread_state_name(t->state), 10);
        print_padded_int(t->run_ticks, 8);
        print_padded_int(t->work, 10);
        zprint("\n");
    }
}

static int parse_uint(char *s){
    if (!s[0]) return -1;
    int n = 0;
    for (int i = 0; s[i]; i++) {
        if (s[i] < '0' || s[i] > '9') return -1;
        n = n * 10 + (s[i] - '0');
    }
    return n;
}

static void spawn_thread(char *kind){
    thread_fn fn = 0;
    if (strcmp(kind, "counter") == 0) fn = demo_counter;
    else if (strcmp(kind, "ticker") == 0) fn = demo_ticker;
    else if (strcmp(kind, "burst") == 0) fn = demo_burst;

    if (!fn) {
        zprint("spawn: unknown kind (use counter, ticker or burst)\n");
        return;
    }
    int id = thread_create(kind, fn, NULL);
    if (id < 0) {
        zprint("spawn: could not create thread (limit or out of memory)\n");
    } else {
        zprint("Started thread ");
        zprint_int(id);
        zprint(" (");
        zprint(kind);
        zprint(")\n");
    }
}

static void kill_thread(char *arg){
    int id = parse_uint(arg);
    if (id < 0) {
        zprint("kill: usage: kill <thread id>\n");
        return;
    }
    int r = thread_kill((uint32_t)id);
    if (r == 0) zprint("Thread killed\n");
    else if (r == -2) zprint("kill: cannot kill the main thread\n");
    else zprint("kill: no such thread\n");
}

/* initrd - show the ramdisk GRUB loaded and how each file was handled */
static void print_hex(uint32_t v){
    char buf[11];
    buf[0] = '0'; buf[1] = 'x';
    for (int i = 0; i < 8; i++) {
        int d = (v >> (28 - 4 * i)) & 0xF;
        buf[2 + i] = d < 10 ? '0' + d : 'a' + (d - 10);
    }
    buf[10] = '\0';
    zprint(buf);
}

static void show_initrd(){
    const initrd_info_t *info = initrd_get_info();
    if (!info->present) {
        zprint("initrd: no image loaded (no GRUB module)\n");
        return;
    }

    zprint("Initrd image:\n");
    zprint("  location : "); print_hex(info->phys_start);
    zprint(" - ");           print_hex(info->phys_end);
    zprint(" (physical)\n");
    print_mem_line("  size     : ", info->size);
    print_mem_count("  files    : ", info->file_count);
    print_mem_count("  extracted: ", info->loaded);
    zprint("  mounted  : " INITRD_MOUNT "\n");

    if (info->file_count > info->recorded) {
        zprint("  (only the first ");
        zprint_int(info->recorded);
        zprint(" entries are tracked)\n");
    }

    zprint("\nNAME                  OFFSET  SIZE    STATUS\n");
    for (uint32_t i = 0; i < info->recorded; i++) {
        const initrd_entry_t *e = initrd_get_entry(i);
        print_padded((char*)e->name, 22);
        print_padded_int(e->offset, 8);
        print_padded_int(e->length, 8);
        zprint((char*)initrd_status_name(e->status));
        zprint("\n");
    }
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
    } else if (strcmp (input, "initrd") == 0){
        show_initrd();
    } else if (strcmp (input, "ps") == 0){
        show_threads();
    } else if (strcmp (input, "mem") == 0){
        show_memory_usage();
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
        zprint("  mem             - show kernel heap and stack usage\n");
        zprint("  initrd          - show the loaded initrd image and its files\n");
        zprint("  ps              - list kernel threads\n");
        zprint("  spawn <kind>    - start a demo thread (counter, ticker, burst)\n");
        zprint("  kill <id>       - stop a thread\n");
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
    } else if (strcmp(arr[0], "spawn") == 0) {
        if (n >= 1) spawn_thread(arr[1]);
        else zprint("spawn: usage: spawn <counter|ticker|burst>\n");
    } else if (strcmp(arr[0], "kill") == 0) {
        if (n >= 1) kill_thread(arr[1]);
        else zprint("kill: usage: kill <thread id>\n");
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


