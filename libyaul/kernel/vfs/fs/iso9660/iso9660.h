/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_KERNEL_FS_ISO9660_H_
#define _YAUL_KERNEL_FS_ISO9660_H_

#include <assert.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include <sys/cdefs.h>
#include <sys/types.h>

#include <cd-block.h>

__BEGIN_DECLS

/* The maximum number of file list entries to read */
#define ISO9660_FILELIST_ENTRIES_COUNT (4096)

/* ISO9660 limitations */
#define ISO_DIR_LEVEL_MAX       8
#define ISO_FILENAME_MAX_LENGTH 11

typedef uint32_t sector_t;

typedef void (*iso9660_sector_read_t)(sector_t sector, void *ptr);

typedef enum iso9660_entry_type {
        ISO9660_ENTRY_TYPE_INVALID   = 0,
        ISO9660_ENTRY_TYPE_FILE      = 1,
        ISO9660_ENTRY_TYPE_DIRECTORY = 2
} iso9660_entry_type_t;

typedef struct {
        iso9660_entry_type_t type;
        char name[ISO_FILENAME_MAX_LENGTH + 1];
        fad_t starting_fad;
        size_t size;
        uint16_t sector_count;
} __aligned(32) iso9660_filelist_entry_t;

static_assert(sizeof(iso9660_filelist_entry_t) == 32);

typedef struct {
        iso9660_sector_read_t sector_read;
        iso9660_filelist_entry_t *entries;
        uint32_t entries_pooled_count;
        uint32_t entries_count;
} iso9660_filelist_t;

typedef void (*iso9660_filelist_walk_t)(iso9660_filelist_t *filelist,
    const iso9660_filelist_entry_t *entry, void *args);

extern void iso9660_filelist_init(iso9660_filelist_t *filelist,
    iso9660_filelist_entry_t *entries,
    iso9660_sector_read_t sector_read,
    int32_t count);

static inline uint32_t __always_inline
iso9660_sector_count_round(uint32_t length)
{
        return (uint32_pow2_round(length, 11) >> 11);
}

extern void iso9660_filelist_default_init(iso9660_filelist_t *filelist,
    iso9660_filelist_entry_t *entries, int32_t count);

extern iso9660_filelist_entry_t *iso9660_entries_alloc(int32_t count);
extern void iso9660_entries_free(iso9660_filelist_entry_t *entries);

extern void iso9660_filelist_root_read(iso9660_filelist_t *filelist);
extern void iso9660_filelist_read(iso9660_filelist_t *filelist,
    const iso9660_filelist_entry_t root_entry);
extern void iso9660_filelist_walk(iso9660_filelist_t *filelist,
    const iso9660_filelist_entry_t *filelist_entry,
    iso9660_filelist_walk_t walker,
    void *args);

void iso9660_sector_read(sector_t sector, void *ptr);
void iso9660_sector_usb_cart_read(sector_t sector, void *ptr);

__END_DECLS

#endif /* _YAUL_KERNEL_FS_ISO9660_H_ */
