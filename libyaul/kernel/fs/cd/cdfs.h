/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_KERNEL_FS_CDFS_H_
#define _YAUL_KERNEL_FS_CDFS_H_

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
#define CDFS_FILELIST_ENTRIES_COUNT (4096)

/* CDFS limitations */
#define ISO_DIR_LEVEL_MAX       8
#define ISO_FILENAME_MAX_LENGTH 11

typedef uint32_t sector_t;

typedef void (*cdfs_sector_read_t)(sector_t sector, void *ptr);

typedef enum cdfs_entry_type {
        CDFS_ENTRY_TYPE_INVALID   = 0,
        CDFS_ENTRY_TYPE_FILE      = 1,
        CDFS_ENTRY_TYPE_DIRECTORY = 2
} cdfs_entry_type_t;

typedef struct {
        cdfs_entry_type_t type;
        char name[ISO_FILENAME_MAX_LENGTH + 1];
        fad_t starting_fad;
        size_t size;
        uint16_t sector_count;
} __aligned(32) cdfs_filelist_entry_t;

static_assert(sizeof(cdfs_filelist_entry_t) == 32);

typedef struct {
        cdfs_sector_read_t sector_read;
        cdfs_filelist_entry_t *entries;
        uint32_t entries_pooled_count;
        uint32_t entries_count;
} cdfs_filelist_t;

typedef void (*cdfs_filelist_walk_t)(cdfs_filelist_t *filelist,
    const cdfs_filelist_entry_t *entry, void *args);

extern void cdfs_filelist_init(cdfs_filelist_t *filelist,
    cdfs_filelist_entry_t *entries,
    cdfs_sector_read_t sector_read,
    int32_t count);

static inline uint32_t __always_inline
cdfs_sector_count_round(uint32_t length)
{
        return (uint32_pow2_round(length, 11) >> 11);
}

extern void cdfs_filelist_default_init(cdfs_filelist_t *filelist,
    cdfs_filelist_entry_t *entries, int32_t count);

extern cdfs_filelist_entry_t *cdfs_entries_alloc(int32_t count);
extern void cdfs_entries_free(cdfs_filelist_entry_t *entries);

extern void cdfs_filelist_root_read(cdfs_filelist_t *filelist);
extern void cdfs_filelist_read(cdfs_filelist_t *filelist,
    const cdfs_filelist_entry_t root_entry);
extern void cdfs_filelist_walk(cdfs_filelist_t *filelist,
    const cdfs_filelist_entry_t *filelist_entry,
    cdfs_filelist_walk_t walker,
    void *args);

void cdfs_sector_read(sector_t sector, void *ptr);
void cdfs_sector_usb_cart_read(sector_t sector, void *ptr);

__END_DECLS

#endif /* _YAUL_KERNEL_FS_CDFS_H_ */
