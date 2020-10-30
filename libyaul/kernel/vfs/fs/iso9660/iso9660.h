/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _ISO9660_H_
#define _ISO9660_H_

#include <sys/cdefs.h>
#include <sys/types.h>

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>


/* XXX: Remove */
#ifdef __linux__
#define __aligned(n) __attribute__ ((aligned(n)))

typedef uint32_t fad_t;
#else
#include <cd-block.h>
#endif /* __linux__ */

__BEGIN_DECLS

/* The maximum number of file list entries to read */
#define ISO9660_FILELIST_ENTRIES_COUNT (4096)

/* Remove */
typedef struct iso9660_filelist_entry iso9660_filelist_entry_t;

typedef void (*iso9660_filelist_walk_t)(const iso9660_filelist_entry_t *, void *);

struct iso9660_filelist_entry {
        char name[16];
        fad_t starting_fad;
        size_t size;
        uint16_t sector_count;
} __aligned(32);

typedef struct {
        iso9660_filelist_entry_t *entries;
        uint32_t entries_pooled_count;
        uint32_t entries_count;
} iso9660_filelist_t;

extern void iso9660_filelist_read(iso9660_filelist_t *, int32_t);
extern void iso9660_filelist_walk(iso9660_filelist_walk_t, void *);

__END_DECLS

#endif /* _ISO9660_H_ */
