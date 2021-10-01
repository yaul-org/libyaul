/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_KERNEL_FS_ISO9660_H_
#define _YAUL_KERNEL_FS_ISO9660_H_

#include <sys/cdefs.h>
#include <sys/types.h>

#include <cd-block.h>

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

__BEGIN_DECLS

/* The maximum number of file list entries to read */
#define ISO9660_FILELIST_ENTRIES_COUNT (4096)

/* ISO9660 limitations */
#define ISO_DIR_LEVEL_MAX       8
#define ISO_FILENAME_MAX_LENGTH 11

/* Remove */
typedef struct iso9660_filelist_entry iso9660_filelist_entry_t;

typedef void (*iso9660_filelist_walk_t)(const iso9660_filelist_entry_t *, void *);

typedef enum iso9660_entry_type {
        ISO9660_ENTRY_TYPE_INVALID   = 0,
        ISO9660_ENTRY_TYPE_FILE      = 1,
        ISO9660_ENTRY_TYPE_DIRECTORY = 2
} iso9660_entry_type_t;

struct iso9660_filelist_entry {
        iso9660_entry_type_t type;
        char name[ISO_FILENAME_MAX_LENGTH + 1];
        fad_t starting_fad;
        size_t size;
        uint16_t sector_count;
} __aligned(32);

static_assert(sizeof(iso9660_filelist_entry_t) == 32);

typedef struct {
        iso9660_filelist_entry_t *entries;
        uint32_t entries_pooled_count;
        uint32_t entries_count;
} iso9660_filelist_t;

extern void iso9660_filelist_root_read(iso9660_filelist_t *filelist,
    int32_t count);
extern void iso9660_filelist_read(const iso9660_filelist_entry_t root_entry,
    iso9660_filelist_t *filelist, int32_t count);
extern void iso9660_filelist_walk(const iso9660_filelist_entry_t *filelist_entry,
    iso9660_filelist_walk_t walker, void *args);

__END_DECLS

#endif /* _YAUL_KERNEL_FS_ISO9660_H_ */
