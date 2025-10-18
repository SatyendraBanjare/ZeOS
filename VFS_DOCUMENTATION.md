# ZeOS Virtual File System (VFS) Implementation

## Overview

The ZeOS Virtual File System (VFS) is a complete heap-based in-memory file system that provides Unix-like file and directory operations. It supports hierarchical directory structures, file read/write operations, and integrates seamlessly with the ZeOS shell.

## Architecture

### Core Components

1. **VFS Node Structure**: Generic nodes that can represent either files or directories
2. **File Descriptors**: Manage open files with position tracking and access control
3. **Path Resolution**: Support for both absolute and relative paths
4. **Memory Management**: Heap-based storage using the kernel's malloc implementation
5. **Shell Integration**: Unix-like commands integrated into the shell

### Data Structures

```c
typedef struct vfs_node {
    file_type_t type;                    // FILE_TYPE_REGULAR or FILE_TYPE_DIRECTORY
    union {
        vfs_file_t file;                 // File-specific data
        vfs_directory_t directory;       // Directory-specific data
    } data;
} vfs_node_t;

typedef struct vfs_file {
    char name[MAX_FILENAME_LENGTH];      // File name (64 chars max)
    uint32_t size;                       // Current file size
    uint32_t allocated_size;             // Allocated memory size
    void* data;                          // File content (heap-allocated)
    file_permissions_t permissions;      // Read/write/execute permissions
    uint32_t creation_time;              // Creation timestamp
    uint32_t modification_time;          // Last modification timestamp
    struct vfs_node* parent;             // Parent directory
} vfs_file_t;

typedef struct vfs_directory {
    char name[MAX_FILENAME_LENGTH];      // Directory name (64 chars max)
    struct vfs_node* children[MAX_CHILDREN]; // Child nodes (32 max)
    uint32_t child_count;                // Number of children
    file_permissions_t permissions;      // Directory permissions
    uint32_t creation_time;              // Creation timestamp
    struct vfs_node* parent;             // Parent directory
} vfs_directory_t;
```

## Features

### File Operations
- **Create**: `vfs_create_file()` or `touch filename`
- **Open**: `vfs_open()` with various flags (O_RDONLY, O_WRONLY, O_RDWR, O_CREAT, O_TRUNC, O_APPEND)
- **Read**: `vfs_read()` or `cat filename`
- **Write**: `vfs_write()` or `echo "text" > filename`
- **Close**: `vfs_close()`
- **Delete**: `vfs_delete_file()` or `rm filename`

### Directory Operations
- **Create**: `vfs_create_directory()` or `mkdir dirname`
- **List**: `vfs_list_directory()` or `ls [path]`
- **Change**: `vfs_change_directory()` or `cd path`
- **Delete**: `vfs_delete_directory()` or `rm dirname` (only if empty)
- **Current Directory**: `vfs_get_current_directory()` or `pwd`

### Path Resolution
- **Absolute Paths**: Start with `/` (e.g., `/home/user/file.txt`)
- **Relative Paths**: Relative to current directory (e.g., `../parent/file.txt`)
- **Special Directories**: `.` (current) and `..` (parent)

## Shell Commands

The VFS integrates with the ZeOS shell providing these commands:

| Command | Syntax | Description |
|---------|--------|-------------|
| `ls` | `ls [path]` | List directory contents |
| `mkdir` | `mkdir <path>` | Create directory |
| `touch` | `touch <file>` | Create empty file |
| `rm` | `rm <path>` | Remove file or empty directory |
| `cd` | `cd <path>` | Change current directory |
| `pwd` | `pwd` | Print working directory |
| `cat` | `cat <file>` | Display file contents |
| `echo` | `echo <text> > <file>` | Write text to file |

## Example Usage

```bash
# List root directory
ls /

# Create directories
mkdir /home
mkdir /home/user
mkdir /tmp

# Create files
touch /home/user/readme.txt
echo "Hello World!" > /home/user/hello.txt

# Navigate directories
cd /home/user
pwd
ls

# Display file contents
cat hello.txt
cat readme.txt

# Navigate with relative paths
cd ../..
pwd
ls

# Remove files and directories
rm /home/user/hello.txt
cd /home
rm user
rm /home
```

## Implementation Details

### Memory Management
- Files are stored in heap-allocated memory using `malloc()`
- Dynamic resizing when files grow beyond allocated size
- Maximum file size limit: 4KB (`MAX_FILE_SIZE`)
- Memory is logically freed when files are deleted (actual free() not implemented yet)

### File System Limits
- Maximum filename length: 64 characters
- Maximum path length: 256 characters
- Maximum children per directory: 32
- Maximum open files: 16
- Maximum file size: 4KB

### Error Handling
The VFS provides comprehensive error codes:
- `VFS_SUCCESS`: Operation completed successfully
- `VFS_ERROR_NOT_FOUND`: File or directory not found
- `VFS_ERROR_EXISTS`: File or directory already exists
- `VFS_ERROR_NO_SPACE`: No space for new entries
- `VFS_ERROR_INVALID`: Invalid parameters
- `VFS_ERROR_NO_PERM`: Permission denied
- `VFS_ERROR_IS_DIR`: Expected file but got directory
- `VFS_ERROR_NOT_DIR`: Expected directory but got file
- `VFS_ERROR_NO_MEM`: Out of memory

### Initialization
The VFS is automatically initialized when the shell starts and creates a default directory structure:
- `/` (root directory)
- `/home` (user home directories)
- `/tmp` (temporary files)
- `/usr` (user programs)
- `/etc` (configuration files)
- `/readme.txt` (sample file with welcome message)

## Future Enhancements

1. **Proper Memory Management**: Implement `free()` function for memory deallocation
2. **Persistence**: Save VFS state to disk or initrd
3. **Extended Attributes**: File metadata like size, timestamps in `ls` output
4. **Symbolic Links**: Support for soft links
5. **File Permissions**: Enforce read/write/execute permissions
6. **Recursive Operations**: `rm -r` for non-empty directories
7. **File Copying**: `cp` command for copying files
8. **File Moving**: `mv` command for moving/renaming files
9. **Wildcards**: Pattern matching in filenames
10. **Pipes and Redirection**: Advanced shell features

## Integration with ZeOS

The VFS seamlessly integrates with other ZeOS components:
- **Heap Manager**: Uses kernel heap for dynamic memory allocation
- **Terminal System**: Provides output through terminal functions
- **Timer System**: Uses RDTSC for timestamps
- **Shell System**: Commands are parsed and executed by shell
- **Keyboard Driver**: User input processed for file operations

This implementation provides a solid foundation for file system operations in ZeOS and demonstrates modern OS concepts like hierarchical file systems, dynamic memory management, and user-friendly command-line interfaces.
