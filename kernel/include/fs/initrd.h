#ifndef INITRD_H
#define INITRD_H

#include <stdint.h>
#include "../multiboot/multiboot.h"

#define INITRD_MAGIC     0xBF
#define INITRD_NAME_LEN  64
#define INITRD_MOUNT     "/initrd"

/* One entry of the header table; matches scripts/make_initrd.py (76 bytes). */
typedef struct initrd_header {
    uint8_t  magic;
    char     name[INITRD_NAME_LEN];
    uint32_t offset;        /* from the start of the image */
    uint32_t length;
} initrd_header_t;

#define INITRD_MAX_ENTRIES 64

typedef enum {
    INITRD_ENTRY_LOADED,        /* extracted into the VFS */
    INITRD_ENTRY_BAD_HEADER,
    INITRD_ENTRY_OUT_OF_RANGE,  /* offset/length outside the image */
    INITRD_ENTRY_TOO_LARGE,     /* bigger than the VFS file limit */
    INITRD_ENTRY_FAILED         /* VFS refused it (no space, no memory, ...) */
} initrd_entry_status_t;

typedef struct initrd_entry {
    char     name[INITRD_NAME_LEN];
    uint32_t offset;
    uint32_t length;
    initrd_entry_status_t status;
} initrd_entry_t;

typedef struct initrd_info {
    int      present;           /* a module was found and its header table was valid */
    uint32_t phys_start;        /* where GRUB placed the image */
    uint32_t phys_end;
    uint32_t size;
    uint32_t file_count;        /* entries in the image */
    uint32_t recorded;          /* entries kept in the table below (<= INITRD_MAX_ENTRIES) */
    uint32_t loaded;            /* entries extracted into the VFS */
} initrd_info_t;

const initrd_info_t  *initrd_get_info(void);
const initrd_entry_t *initrd_get_entry(uint32_t index);   /* NULL past the end */
const char *initrd_status_name(initrd_entry_status_t status);

/* Find the first GRUB module, and unpack it into the VFS under /initrd.
 * Returns the number of files extracted, or a negative VFS error / -1 if there
 * is no usable initrd. Call after vfs_init(). */
int initrd_load(struct multiboot_info *info);

#endif
