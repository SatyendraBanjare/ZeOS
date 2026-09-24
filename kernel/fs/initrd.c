#include "../include/fs/initrd.h"
#include "../include/fs/fs.h"
#include "../include/multiboot/multiboot_util.h"

/* boot.asm maps the first 4 MB of physical memory at 0xC0000000 */
#define MAPPED_PHYSICAL_LIMIT 0x400000

static initrd_info_t info_state;
static initrd_entry_t entries[INITRD_MAX_ENTRIES];

const initrd_info_t *initrd_get_info(void) { return &info_state; }

const initrd_entry_t *initrd_get_entry(uint32_t index) {
    return (index < info_state.recorded) ? &entries[index] : NULL;
}

const char *initrd_status_name(initrd_entry_status_t status) {
    switch (status) {
        case INITRD_ENTRY_LOADED:       return "loaded";
        case INITRD_ENTRY_BAD_HEADER:   return "bad header";
        case INITRD_ENTRY_OUT_OF_RANGE: return "out of range";
        case INITRD_ENTRY_TOO_LARGE:    return "too large";
        default:                        return "failed";
    }
}

static void record_entry(uint32_t i, const initrd_header_t *h, initrd_entry_status_t status) {
    if (i >= INITRD_MAX_ENTRIES) return;
    initrd_entry_t *e = &entries[i];
    int n = 0;
    while (n < INITRD_NAME_LEN - 1 && h->name[n]) { e->name[n] = h->name[n]; n++; }
    e->name[n] = '\0';
    e->offset = h->offset;
    e->length = h->length;
    e->status = status;
    if (i + 1 > info_state.recorded) info_state.recorded = i + 1;
}

_Static_assert(sizeof(initrd_header_t) == 76, "initrd header layout changed");

/* Create every directory on the way to `path` (not the last component). */
static void make_parent_dirs(const char *path) {
    char partial[MAX_PATH_LENGTH];
    int len = 0;

    for (int i = 0; path[i] && len < MAX_PATH_LENGTH - 1; i++) {
        if (path[i] == '/' && i > 0) {
            partial[len] = '\0';
            vfs_create_directory(partial, PERM_READ | PERM_WRITE | PERM_EXECUTE);
        }
        partial[len++] = path[i];
    }
}

static int extract_file(const char *name, const uint8_t *data, uint32_t length) {
    char path[MAX_PATH_LENGTH];
    int len = 0;

    const char *mount = INITRD_MOUNT "/";
    for (int i = 0; mount[i]; i++) path[len++] = mount[i];
    for (int i = 0; name[i] && len < MAX_PATH_LENGTH - 1; i++) path[len++] = name[i];
    path[len] = '\0';

    make_parent_dirs(path);

    int r = vfs_create_file(path, PERM_READ | PERM_WRITE);
    if (r != VFS_SUCCESS && r != VFS_ERROR_EXISTS) return r;

    int fd = vfs_open(path, O_WRONLY | O_TRUNC);
    if (fd < 0) return fd;
    int written = (length > 0) ? vfs_write(fd, data, length) : 0;
    vfs_close(fd);

    return (written == (int)length) ? VFS_SUCCESS : VFS_ERROR_NO_SPACE;
}

int initrd_load(struct multiboot_info *info) {
    if (!info || !(info->flags & MULTIBOOT_INFO_MODS) || info->mods_count == 0) {
        print_log("initrd: no GRUB module found\n");
        return -1;
    }

    multiboot_module_t *module = (multiboot_module_t *) p_to_v(info->mods_addr);
    uint32_t size = module->mod_end - module->mod_start;

    if (module->mod_end > MAPPED_PHYSICAL_LIMIT) {
        print_log("initrd: module lies beyond the mapped 4 MB, not loaded\n");
        return -1;
    }
    if (size < sizeof(uint32_t)) {
        print_log("initrd: image too small\n");
        return -1;
    }

    info_state.phys_start = module->mod_start;
    info_state.phys_end = module->mod_end;
    info_state.size = size;

    const uint8_t *image = (const uint8_t *) p_to_v(module->mod_start);
    uint32_t count = *(const uint32_t *) image;
    const initrd_header_t *headers = (const initrd_header_t *)(image + sizeof(uint32_t));

    if (count > (size - sizeof(uint32_t)) / sizeof(initrd_header_t)) {
        print_log("initrd: corrupt header table\n");
        return -1;
    }

    info_state.present = 1;
    info_state.file_count = count;
    info_state.recorded = 0;
    info_state.loaded = 0;

    int r = vfs_create_directory(INITRD_MOUNT, PERM_READ | PERM_WRITE | PERM_EXECUTE);
    if (r != VFS_SUCCESS && r != VFS_ERROR_EXISTS) return r;

    int loaded = 0;
    for (uint32_t i = 0; i < count; i++) {
        const initrd_header_t *h = &headers[i];

        if (h->magic != INITRD_MAGIC || h->name[INITRD_NAME_LEN - 1] != '\0' || h->name[0] == '\0') {
            print_log("initrd: skipping entry with bad header\n");
            record_entry(i, h, INITRD_ENTRY_BAD_HEADER);
            continue;
        }
        if (h->offset > size || h->length > size - h->offset) {
            print_log("initrd: skipping entry outside the image: ");
            print_log((char *)h->name);
            print_log("\n");
            record_entry(i, h, INITRD_ENTRY_OUT_OF_RANGE);
            continue;
        }
        if (h->length > MAX_FILE_SIZE) {
            print_log("initrd: skipping file larger than the VFS limit: ");
            print_log((char *)h->name);
            print_log("\n");
            record_entry(i, h, INITRD_ENTRY_TOO_LARGE);
            continue;
        }

        if (extract_file(h->name, image + h->offset, h->length) == VFS_SUCCESS) {
            loaded++;
            record_entry(i, h, INITRD_ENTRY_LOADED);
        } else {
            record_entry(i, h, INITRD_ENTRY_FAILED);
            print_log("initrd: could not extract ");
            print_log((char *)h->name);
            print_log("\n");
        }
    }

    info_state.loaded = loaded;
    print_log("initrd: extracted files: ");
    print_log_int(loaded, 10);
    print_log("\n");
    return loaded;
}
