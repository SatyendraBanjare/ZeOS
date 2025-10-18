#include "../include/fs/fs.h"

// Global VFS state
static vfs_state_t vfs_state;

/**
 * Initialize the Virtual File System
 */
int vfs_init(void) {
    // Initialize VFS state
    vfs_state.root = NULL;
    vfs_state.current_directory = NULL;
    
    // Clear open files array
    for (int i = 0; i < MAX_OPEN_FILES; i++) {
        vfs_state.open_files[i].is_open = 0;
        vfs_state.open_files[i].node = NULL;
        vfs_state.open_files[i].position = 0;
        vfs_state.open_files[i].flags = 0;
    }
    
    // Create root directory
    vfs_state.root = vfs_create_node(FILE_TYPE_DIRECTORY, "/", NULL);
    if (!vfs_state.root) {
        return VFS_ERROR_NO_MEM;
    }
    
    vfs_state.current_directory = vfs_state.root;
    
    // Initialize current path
    vfs_state.current_path[0] = '/';
    vfs_state.current_path[1] = '\0';
    
    // Set up operation pointers
    vfs_state.ops.create_file = vfs_create_file;
    vfs_state.ops.create_directory = vfs_create_directory;
    vfs_state.ops.delete_file = vfs_delete_file;
    vfs_state.ops.delete_directory = vfs_delete_directory;
    vfs_state.ops.open_file = vfs_open;
    vfs_state.ops.close_file = vfs_close;
    vfs_state.ops.read_file = vfs_read;
    vfs_state.ops.write_file = vfs_write;
    vfs_state.ops.list_directory = vfs_list_directory;
    vfs_state.ops.change_directory = vfs_change_directory;
    vfs_state.ops.get_current_directory = vfs_get_current_directory;
    vfs_state.ops.find_node = vfs_find_node;
    
    return VFS_SUCCESS;
}

/**
 * Create a new VFS node
 */
vfs_node_t* vfs_create_node(file_type_t type, const char* name, vfs_node_t* parent) {
    vfs_node_t* node = (vfs_node_t*)malloc(sizeof(vfs_node_t));
    if (!node) {
        return NULL;
    }
    
    node->type = type;
    
    if (type == FILE_TYPE_DIRECTORY) {
        // Initialize directory
        int name_len = strlen((char*)name);
        if (name_len >= MAX_FILENAME_LENGTH) {
            name_len = MAX_FILENAME_LENGTH - 1;
        }
        
        for (int i = 0; i < name_len; i++) {
            node->data.directory.name[i] = name[i];
        }
        node->data.directory.name[name_len] = '\0';
        
        node->data.directory.child_count = 0;
        node->data.directory.parent = parent;
        node->data.directory.permissions = PERM_READ | PERM_WRITE | PERM_EXECUTE;
        node->data.directory.creation_time = rdtsc(); // Use timer for timestamp
        
        // Initialize children array
        for (int i = 0; i < MAX_CHILDREN; i++) {
            node->data.directory.children[i] = NULL;
        }
    } else {
        // Initialize file
        int name_len = strlen((char*)name);
        if (name_len >= MAX_FILENAME_LENGTH) {
            name_len = MAX_FILENAME_LENGTH - 1;
        }
        
        for (int i = 0; i < name_len; i++) {
            node->data.file.name[i] = name[i];
        }
        node->data.file.name[name_len] = '\0';
        
        node->data.file.size = 0;
        node->data.file.allocated_size = 0;
        node->data.file.data = NULL;
        node->data.file.parent = parent;
        node->data.file.permissions = PERM_READ | PERM_WRITE;
        node->data.file.creation_time = rdtsc();
        node->data.file.modification_time = rdtsc();
    }
    
    return node;
}

/**
 * Destroy a VFS node and free its memory
 */
void vfs_destroy_node(vfs_node_t* node) {
    if (!node) return;
    
    if (node->type == FILE_TYPE_DIRECTORY) {
        // Recursively destroy children
        for (int i = 0; i < node->data.directory.child_count; i++) {
            if (node->data.directory.children[i]) {
                vfs_destroy_node(node->data.directory.children[i]);
            }
        }
    } else {
        // Free file data if allocated
        if (node->data.file.data) {
            // Note: In a real implementation, you'd want a proper free() function
            // For now, we'll just set it to NULL
            node->data.file.data = NULL;
        }
    }
    
    // Free the node itself
    // Note: In a real implementation, you'd call free(node)
    // For now, we'll just set it to NULL in the caller
}

/**
 * Add a child node to a directory
 */
int vfs_add_child(vfs_node_t* parent, vfs_node_t* child) {
    if (!parent || !child || parent->type != FILE_TYPE_DIRECTORY) {
        return VFS_ERROR_INVALID;
    }
    
    if (parent->data.directory.child_count >= MAX_CHILDREN) {
        return VFS_ERROR_NO_SPACE;
    }
    
    // Check if child with same name already exists
    const char* child_name = (child->type == FILE_TYPE_DIRECTORY) ? 
        child->data.directory.name : child->data.file.name;
        
    for (int i = 0; i < parent->data.directory.child_count; i++) {
        vfs_node_t* existing = parent->data.directory.children[i];
        if (existing) {
            const char* existing_name = (existing->type == FILE_TYPE_DIRECTORY) ?
                existing->data.directory.name : existing->data.file.name;
            if (strcmp((char*)existing_name, (char*)child_name) == 0) {
                return VFS_ERROR_EXISTS;
            }
        }
    }
    
    // Add child
    parent->data.directory.children[parent->data.directory.child_count] = child;
    parent->data.directory.child_count++;
    
    return VFS_SUCCESS;
}

/**
 * Remove a child from a directory
 */
int vfs_remove_child(vfs_node_t* parent, const char* name) {
    if (!parent || !name || parent->type != FILE_TYPE_DIRECTORY) {
        return VFS_ERROR_INVALID;
    }
    
    for (int i = 0; i < parent->data.directory.child_count; i++) {
        vfs_node_t* child = parent->data.directory.children[i];
        if (child) {
            const char* child_name = (child->type == FILE_TYPE_DIRECTORY) ?
                child->data.directory.name : child->data.file.name;
            if (strcmp((char*)child_name, (char*)name) == 0) {
                // Remove child by shifting remaining children
                for (int j = i; j < parent->data.directory.child_count - 1; j++) {
                    parent->data.directory.children[j] = parent->data.directory.children[j + 1];
                }
                parent->data.directory.children[parent->data.directory.child_count - 1] = NULL;
                parent->data.directory.child_count--;
                
                vfs_destroy_node(child);
                return VFS_SUCCESS;
            }
        }
    }
    
    return VFS_ERROR_NOT_FOUND;
}

/**
 * Find a child in a directory by name
 */
vfs_node_t* vfs_find_child(vfs_node_t* parent, const char* name) {
    if (!parent || !name || parent->type != FILE_TYPE_DIRECTORY) {
        return NULL;
    }
    
    for (int i = 0; i < parent->data.directory.child_count; i++) {
        vfs_node_t* child = parent->data.directory.children[i];
        if (child) {
            const char* child_name = (child->type == FILE_TYPE_DIRECTORY) ?
                child->data.directory.name : child->data.file.name;
            if (strcmp((char*)child_name, (char*)name) == 0) {
                return child;
            }
        }
    }
    
    return NULL;
}

/**
 * Split a path into components
 */
char** vfs_split_path(const char* path, uint32_t* count) {
    *count = 0;
    if (!path) return NULL;
    
    // Count components
    int len = strlen((char*)path);
    for (int i = 0; i < len; i++) {
        if (path[i] == '/' && i > 0 && path[i-1] != '/') {
            (*count)++;
        }
    }
    if (len > 0 && path[len-1] != '/') {
        (*count)++;
    }
    
    if (*count == 0) return NULL;
    
    // Allocate array of string pointers
    char** components = (char**)malloc(sizeof(char*) * (*count));
    if (!components) {
        *count = 0;
        return NULL;
    }
    
    // Parse components
    int comp_idx = 0;
    int start = 0;
    if (path[0] == '/') start = 1; // Skip leading slash
    
    for (int i = start; i <= len; i++) {
        if (path[i] == '/' || path[i] == '\0') {
            if (i > start) {
                int comp_len = i - start;
                components[comp_idx] = (char*)malloc(comp_len + 1);
                if (components[comp_idx]) {
                    for (int j = 0; j < comp_len; j++) {
                        components[comp_idx][j] = path[start + j];
                    }
                    components[comp_idx][comp_len] = '\0';
                    comp_idx++;
                }
            }
            start = i + 1;
        }
    }
    
    return components;
}

/**
 * Free path components
 */
void vfs_free_path_components(char** components, uint32_t count) {
    if (!components) return;
    
    for (uint32_t i = 0; i < count; i++) {
        if (components[i]) {
            // Note: In a real implementation, call free(components[i])
            components[i] = NULL;
        }
    }
    // Note: In a real implementation, call free(components)
}

/**
 * Find a node by path
 */
vfs_node_t* vfs_find_node(const char* path) {
    if (!path) return NULL;
    
    vfs_node_t* current;
    
    // Determine starting point
    if (path[0] == '/') {
        current = vfs_state.root; // Absolute path
    } else {
        current = vfs_state.current_directory; // Relative path
    }
    
    if (strcmp((char*)path, "/") == 0) {
        return vfs_state.root;
    }
    
    uint32_t count;
    char** components = vfs_split_path(path, &count);
    
    if (!components) return current;
    
    // Traverse path
    for (uint32_t i = 0; i < count; i++) {
        if (strcmp(components[i], ".") == 0) {
            continue; // Current directory
        } else if (strcmp(components[i], "..") == 0) {
            // Parent directory
            if (current->type == FILE_TYPE_DIRECTORY && current->data.directory.parent) {
                current = current->data.directory.parent;
            }
        } else {
            // Find child
            current = vfs_find_child(current, components[i]);
            if (!current) {
                vfs_free_path_components(components, count);
                return NULL;
            }
        }
    }
    
    vfs_free_path_components(components, count);
    return current;
}

/**
 * Create a file
 */
int vfs_create_file(const char* path, file_permissions_t permissions) {
    if (!path) return VFS_ERROR_INVALID;
    
    // Find parent directory
    char parent_path[MAX_PATH_LENGTH];
    char filename[MAX_FILENAME_LENGTH];
    
    int len = strlen((char*)path);
    int last_slash = -1;
    
    for (int i = len - 1; i >= 0; i--) {
        if (path[i] == '/') {
            last_slash = i;
            break;
        }
    }
    
    if (last_slash == -1) {
        // No slash found, use current directory
        parent_path[0] = '.';
        parent_path[1] = '\0';
        int name_len = len;
        if (name_len >= MAX_FILENAME_LENGTH) name_len = MAX_FILENAME_LENGTH - 1;
        for (int i = 0; i < name_len; i++) {
            filename[i] = path[i];
        }
        filename[name_len] = '\0';
    } else {
        // Extract parent path and filename
        int parent_len = last_slash == 0 ? 1 : last_slash;
        if (parent_len >= MAX_PATH_LENGTH) parent_len = MAX_PATH_LENGTH - 1;
        for (int i = 0; i < parent_len; i++) {
            parent_path[i] = path[i];
        }
        parent_path[parent_len] = '\0';
        
        int name_len = len - last_slash - 1;
        if (name_len >= MAX_FILENAME_LENGTH) name_len = MAX_FILENAME_LENGTH - 1;
        for (int i = 0; i < name_len; i++) {
            filename[i] = path[last_slash + 1 + i];
        }
        filename[name_len] = '\0';
    }
    
    vfs_node_t* parent = vfs_find_node(parent_path);
    if (!parent || parent->type != FILE_TYPE_DIRECTORY) {
        return VFS_ERROR_NOT_FOUND;
    }
    
    // Create file node
    vfs_node_t* file_node = vfs_create_node(FILE_TYPE_REGULAR, filename, parent);
    if (!file_node) {
        return VFS_ERROR_NO_MEM;
    }
    
    file_node->data.file.permissions = permissions;
    
    // Add to parent directory
    int result = vfs_add_child(parent, file_node);
    if (result != VFS_SUCCESS) {
        vfs_destroy_node(file_node);
        return result;
    }
    
    return VFS_SUCCESS;
}

/**
 * Create a directory
 */
int vfs_create_directory(const char* path, file_permissions_t permissions) {
    if (!path) return VFS_ERROR_INVALID;
    
    // Find parent directory
    char parent_path[MAX_PATH_LENGTH];
    char dirname[MAX_FILENAME_LENGTH];
    
    int len = strlen((char*)path);
    int last_slash = -1;
    
    for (int i = len - 1; i >= 0; i--) {
        if (path[i] == '/') {
            last_slash = i;
            break;
        }
    }
    
    if (last_slash == -1) {
        // No slash found, use current directory
        parent_path[0] = '.';
        parent_path[1] = '\0';
        int name_len = len;
        if (name_len >= MAX_FILENAME_LENGTH) name_len = MAX_FILENAME_LENGTH - 1;
        for (int i = 0; i < name_len; i++) {
            dirname[i] = path[i];
        }
        dirname[name_len] = '\0';
    } else {
        // Extract parent path and dirname
        int parent_len = last_slash == 0 ? 1 : last_slash;
        if (parent_len >= MAX_PATH_LENGTH) parent_len = MAX_PATH_LENGTH - 1;
        for (int i = 0; i < parent_len; i++) {
            parent_path[i] = path[i];
        }
        parent_path[parent_len] = '\0';
        
        int name_len = len - last_slash - 1;
        if (name_len >= MAX_FILENAME_LENGTH) name_len = MAX_FILENAME_LENGTH - 1;
        for (int i = 0; i < name_len; i++) {
            dirname[i] = path[last_slash + 1 + i];
        }
        dirname[name_len] = '\0';
    }
    
    vfs_node_t* parent = vfs_find_node(parent_path);
    if (!parent || parent->type != FILE_TYPE_DIRECTORY) {
        return VFS_ERROR_NOT_FOUND;
    }
    
    // Create directory node
    vfs_node_t* dir_node = vfs_create_node(FILE_TYPE_DIRECTORY, dirname, parent);
    if (!dir_node) {
        return VFS_ERROR_NO_MEM;
    }
    
    dir_node->data.directory.permissions = permissions;
    
    // Add to parent directory
    int result = vfs_add_child(parent, dir_node);
    if (result != VFS_SUCCESS) {
        vfs_destroy_node(dir_node);
        return result;
    }
    
    return VFS_SUCCESS;
}

/**
 * Open a file and return file descriptor
 */
int vfs_open(const char* path, uint32_t flags) {
    if (!path) return VFS_ERROR_INVALID;
    
    vfs_node_t* node = vfs_find_node(path);
    
    // Handle file creation if O_CREAT flag is set
    if (!node && (flags & O_CREAT)) {
        int result = vfs_create_file(path, PERM_READ | PERM_WRITE);
        if (result != VFS_SUCCESS) {
            return result;
        }
        node = vfs_find_node(path);
    }
    
    if (!node) {
        return VFS_ERROR_NOT_FOUND;
    }
    
    if (node->type != FILE_TYPE_REGULAR) {
        return VFS_ERROR_IS_DIR;
    }
    
    // Find free file descriptor
    int fd = -1;
    for (int i = 0; i < MAX_OPEN_FILES; i++) {
        if (!vfs_state.open_files[i].is_open) {
            fd = i;
            break;
        }
    }
    
    if (fd == -1) {
        return VFS_ERROR_NO_SPACE;
    }
    
    // Initialize file descriptor
    vfs_state.open_files[fd].node = node;
    vfs_state.open_files[fd].flags = flags;
    vfs_state.open_files[fd].is_open = 1;
    
    if (flags & O_TRUNC) {
        // Truncate file
        node->data.file.size = 0;
        vfs_state.open_files[fd].position = 0;
    } else if (flags & O_APPEND) {
        // Position at end
        vfs_state.open_files[fd].position = node->data.file.size;
    } else {
        // Position at beginning
        vfs_state.open_files[fd].position = 0;
    }
    
    return fd;
}

/**
 * Close a file
 */
int vfs_close(int fd) {
    if (fd < 0 || fd >= MAX_OPEN_FILES || !vfs_state.open_files[fd].is_open) {
        return VFS_ERROR_INVALID;
    }
    
    vfs_state.open_files[fd].is_open = 0;
    vfs_state.open_files[fd].node = NULL;
    vfs_state.open_files[fd].position = 0;
    vfs_state.open_files[fd].flags = 0;
    
    return VFS_SUCCESS;
}

/**
 * Read from a file
 */
int vfs_read(int fd, void* buffer, uint32_t size) {
    if (fd < 0 || fd >= MAX_OPEN_FILES || !vfs_state.open_files[fd].is_open) {
        return VFS_ERROR_INVALID;
    }
    
    if (!buffer) {
        return VFS_ERROR_INVALID;
    }
    
    file_descriptor_t* file_desc = &vfs_state.open_files[fd];
    vfs_node_t* node = file_desc->node;
    
    if (!(file_desc->flags & (O_RDONLY | O_RDWR))) {
        return VFS_ERROR_NO_PERM;
    }
    
    if (file_desc->position >= node->data.file.size) {
        return 0; // EOF
    }
    
    uint32_t bytes_to_read = size;
    if (file_desc->position + bytes_to_read > node->data.file.size) {
        bytes_to_read = node->data.file.size - file_desc->position;
    }
    
    if (node->data.file.data) {
        memory_copy((uint8_t*)node->data.file.data + file_desc->position, 
                   (uint8_t*)buffer, bytes_to_read);
    }
    
    file_desc->position += bytes_to_read;
    
    return bytes_to_read;
}

/**
 * Write to a file
 */
int vfs_write(int fd, const void* buffer, uint32_t size) {
    if (fd < 0 || fd >= MAX_OPEN_FILES || !vfs_state.open_files[fd].is_open) {
        return VFS_ERROR_INVALID;
    }
    
    if (!buffer) {
        return VFS_ERROR_INVALID;
    }
    
    file_descriptor_t* file_desc = &vfs_state.open_files[fd];
    vfs_node_t* node = file_desc->node;
    
    if (!(file_desc->flags & (O_WRONLY | O_RDWR))) {
        return VFS_ERROR_NO_PERM;
    }
    
    uint32_t new_size = file_desc->position + size;
    
    // Allocate or reallocate file data if needed
    if (new_size > node->data.file.allocated_size) {
        uint32_t new_allocated_size = new_size;
        if (new_allocated_size > MAX_FILE_SIZE) {
            new_allocated_size = MAX_FILE_SIZE;
        }
        
        void* new_data = malloc(new_allocated_size);
        if (!new_data) {
            return VFS_ERROR_NO_MEM;
        }
        
        if (node->data.file.data && node->data.file.size > 0) {
            memory_copy((uint8_t*)node->data.file.data, (uint8_t*)new_data, node->data.file.size);
        }
        
        node->data.file.data = new_data;
        node->data.file.allocated_size = new_allocated_size;
    }
    
    if (new_size > MAX_FILE_SIZE) {
        size = MAX_FILE_SIZE - file_desc->position;
        new_size = MAX_FILE_SIZE;
    }
    
    // Copy data
    memory_copy((uint8_t*)buffer, 
               (uint8_t*)node->data.file.data + file_desc->position, size);
    
    file_desc->position += size;
    
    if (new_size > node->data.file.size) {
        node->data.file.size = new_size;
        node->data.file.modification_time = rdtsc();
    }
    
    return size;
}

/**
 * Change current directory
 */
int vfs_change_directory(const char* path) {
    if (!path) return VFS_ERROR_INVALID;
    
    vfs_node_t* node = vfs_find_node(path);
    if (!node) {
        return VFS_ERROR_NOT_FOUND;
    }
    
    if (node->type != FILE_TYPE_DIRECTORY) {
        return VFS_ERROR_NOT_DIR;
    }
    
    vfs_state.current_directory = node;
    
    // Update current path
    // For simplicity, we'll just copy the path (in a real implementation, you'd normalize it)
    int len = strlen((char*)path);
    if (len >= MAX_PATH_LENGTH) len = MAX_PATH_LENGTH - 1;
    for (int i = 0; i < len; i++) {
        vfs_state.current_path[i] = path[i];
    }
    vfs_state.current_path[len] = '\0';
    
    return VFS_SUCCESS;
}

/**
 * Get current directory path
 */
char* vfs_get_current_directory(void) {
    return vfs_state.current_path;
}

/**
 * Delete a file
 */
int vfs_delete_file(const char* path) {
    if (!path) return VFS_ERROR_INVALID;
    
    vfs_node_t* node = vfs_find_node(path);
    if (!node) {
        return VFS_ERROR_NOT_FOUND;
    }
    
    if (node->type != FILE_TYPE_REGULAR) {
        return VFS_ERROR_IS_DIR;
    }
    
    vfs_node_t* parent = node->data.file.parent;
    if (!parent) {
        return VFS_ERROR_INVALID;
    }
    
    return vfs_remove_child(parent, node->data.file.name);
}

/**
 * Delete a directory
 */
int vfs_delete_directory(const char* path) {
    if (!path) return VFS_ERROR_INVALID;
    
    vfs_node_t* node = vfs_find_node(path);
    if (!node) {
        return VFS_ERROR_NOT_FOUND;
    }
    
    if (node->type != FILE_TYPE_DIRECTORY) {
        return VFS_ERROR_NOT_DIR;
    }
    
    // Check if directory is empty
    if (node->data.directory.child_count > 0) {
        return VFS_ERROR_NO_PERM; // Directory not empty
    }
    
    vfs_node_t* parent = node->data.directory.parent;
    if (!parent) {
        return VFS_ERROR_INVALID; // Can't delete root
    }
    
    return vfs_remove_child(parent, node->data.directory.name);
}

/**
 * List directory contents
 */
int vfs_list_directory(const char* path, char*** entries, uint32_t* count) {
    if (!entries || !count) return VFS_ERROR_INVALID;
    
    vfs_node_t* node;
    if (path) {
        node = vfs_find_node(path);
    } else {
        node = vfs_state.current_directory;
    }
    
    if (!node || node->type != FILE_TYPE_DIRECTORY) {
        return VFS_ERROR_NOT_DIR;
    }
    
    *count = node->data.directory.child_count;
    if (*count == 0) {
        *entries = NULL;
        return VFS_SUCCESS;
    }
    
    *entries = (char**)malloc(sizeof(char*) * (*count));
    if (!*entries) {
        return VFS_ERROR_NO_MEM;
    }
    
    for (uint32_t i = 0; i < *count; i++) {
        vfs_node_t* child = node->data.directory.children[i];
        const char* name = (child->type == FILE_TYPE_DIRECTORY) ?
            child->data.directory.name : child->data.file.name;
        
        int name_len = strlen((char*)name);
        (*entries)[i] = (char*)malloc(name_len + 1);
        if ((*entries)[i]) {
            for (int j = 0; j < name_len; j++) {
                (*entries)[i][j] = name[j];
            }
            (*entries)[i][name_len] = '\0';
        }
    }
    
    return VFS_SUCCESS;
}
