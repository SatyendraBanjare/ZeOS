#ifndef FS_H
#define FS_H

#include <stdint.h>
#include <stddef.h>
#include "../cpu/memory/heap.h"
#include "../common/helper.h"
#include "../terminal/terminal.h"
#include "../cpu/timer.h"

#define MAX_FILENAME_LENGTH 64
#define MAX_PATH_LENGTH 256
#define MAX_FILE_SIZE 4096
#define MAX_CHILDREN 32
#define MAX_OPEN_FILES 16

// File types
typedef enum {
    FILE_TYPE_REGULAR = 1,
    FILE_TYPE_DIRECTORY = 2
} file_type_t;

// File permissions
typedef enum {
    PERM_READ = 1,
    PERM_WRITE = 2,
    PERM_EXECUTE = 4
} file_permissions_t;

// Forward declarations
struct vfs_node;
struct vfs_directory;

// File structure
typedef struct vfs_file {
    char name[MAX_FILENAME_LENGTH];
    uint32_t size;
    uint32_t allocated_size;
    void* data;
    file_permissions_t permissions;
    uint32_t creation_time;
    uint32_t modification_time;
    struct vfs_node* parent;
} vfs_file_t;

// Directory structure
typedef struct vfs_directory {
    char name[MAX_FILENAME_LENGTH];
    struct vfs_node* children[MAX_CHILDREN];
    uint32_t child_count;
    file_permissions_t permissions;
    uint32_t creation_time;
    struct vfs_node* parent;
} vfs_directory_t;

// Generic VFS node
typedef struct vfs_node {
    file_type_t type;
    union {
        vfs_file_t file;
        vfs_directory_t directory;
    } data;
} vfs_node_t;

// File descriptor for open files
typedef struct file_descriptor {
    vfs_node_t* node;
    uint32_t position;
    uint32_t flags;
    int is_open;
} file_descriptor_t;

// VFS operations
typedef struct vfs_operations {
    int (*create_file)(const char* path, file_permissions_t permissions);
    int (*create_directory)(const char* path, file_permissions_t permissions);
    int (*delete_file)(const char* path);
    int (*delete_directory)(const char* path);
    int (*open_file)(const char* path, uint32_t flags);
    int (*close_file)(int fd);
    int (*read_file)(int fd, void* buffer, uint32_t size);
    int (*write_file)(int fd, const void* buffer, uint32_t size);
    int (*list_directory)(const char* path, char*** entries, uint32_t* count);
    int (*change_directory)(const char* path);
    char* (*get_current_directory)(void);
    vfs_node_t* (*find_node)(const char* path);
} vfs_operations_t;

// Global VFS state
typedef struct vfs_state {
    vfs_node_t* root;
    vfs_node_t* current_directory;
    file_descriptor_t open_files[MAX_OPEN_FILES];
    char current_path[MAX_PATH_LENGTH];
    vfs_operations_t ops;
} vfs_state_t;

// File operation flags
#define O_RDONLY 0x01
#define O_WRONLY 0x02
#define O_RDWR   0x03
#define O_CREAT  0x04
#define O_TRUNC  0x08
#define O_APPEND 0x10

// Error codes
#define VFS_SUCCESS          0
#define VFS_ERROR_NOT_FOUND -1
#define VFS_ERROR_EXISTS    -2
#define VFS_ERROR_NO_SPACE  -3
#define VFS_ERROR_INVALID   -4
#define VFS_ERROR_NO_PERM   -5
#define VFS_ERROR_IS_DIR    -6
#define VFS_ERROR_NOT_DIR   -7
#define VFS_ERROR_NO_MEM    -8

// Function declarations
int vfs_init(void);
int vfs_create_file(const char* path, file_permissions_t permissions);
int vfs_create_directory(const char* path, file_permissions_t permissions);
int vfs_delete_file(const char* path);
int vfs_delete_directory(const char* path);
int vfs_open(const char* path, uint32_t flags);
int vfs_close(int fd);
int vfs_read(int fd, void* buffer, uint32_t size);
int vfs_write(int fd, const void* buffer, uint32_t size);
int vfs_list_directory(const char* path, char*** entries, uint32_t* count);
int vfs_change_directory(const char* path);
char* vfs_get_current_directory(void);
vfs_node_t* vfs_find_node(const char* path);

// Utility functions
char* vfs_normalize_path(const char* path);
char** vfs_split_path(const char* path, uint32_t* count);
void vfs_free_path_components(char** components, uint32_t count);
vfs_node_t* vfs_create_node(file_type_t type, const char* name, vfs_node_t* parent);
void vfs_destroy_node(vfs_node_t* node);
int vfs_add_child(vfs_node_t* parent, vfs_node_t* child);
int vfs_remove_child(vfs_node_t* parent, const char* name);
vfs_node_t* vfs_find_child(vfs_node_t* parent, const char* name);

// Shell integration functions
void vfs_shell_ls(const char* path);
void vfs_shell_mkdir(const char* path);
void vfs_shell_touch(const char* path);
void vfs_shell_rm(const char* path);
void vfs_shell_cd(const char* path);
void vfs_shell_pwd(void);
void vfs_shell_cat(const char* path);
void vfs_shell_echo(const char* text, const char* path);

#endif
