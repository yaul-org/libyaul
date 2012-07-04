/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Dan Potter
 * Andrew Kieschnick
 * Bero
 * Lawrence Sebald
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <stdlib.h>

#include "iso9660-internal.h"

#include "iso9660.h"

#define DEBUG
#define __unused __attribute__ ((unused))

typedef struct {
        iso9660_dirent_t *root;
        uint16_t logical_block_size;
} iso9660_mnt_t;

static iso9660_dirent_t *iso9660_find(iso9660_mnt_t *, const char *, bool);
static iso9660_dirent_t *iso9660_find_object(iso9660_mnt_t *, const char *, size_t, bool, iso9660_dirent_t *);

static void *bread(uint16_t);

void *
iso9660_open(void *p, const char *fn,
    int mode __unused)
{
        bool directory;
        iso9660_mnt_t *mnt;
        iso9660_dirent_t *xp;

        mnt = (iso9660_mnt_t *)p;
        directory = false;
        if ((xp = iso9660_find(mnt, fn, directory)) == NULL) {
                errno = ENOENT;
                return NULL;
        }

        mnt = mnt;
        return xp;
}

void
romdisk_close(void *fh __unused)
{
}

ssize_t
iso9660_read(void *p, void *buf, size_t bytes)
{
        iso9660_mnt_t *mnt __unused;
        iso9660_dirent_t *xp __unused;

        mnt = (iso9660_mnt_t *)p;

        return 0;
}

void *
iso9660_mount(const char *mnt_point __unused,
    const uint8_t *image __unused)
{
        struct iso_primary_descriptor *pdp;
        struct iso_volume_descriptor *vdp;

        struct root_directory_record *rootp;
        iso9660_mnt_t *mnt;

        vdp = bread(0x10);
        /* Must be a "Primary Volume Descriptor" */
        if (isonum_711(vdp->type) != ISO_VD_PRIMARY)
                return NULL;

        if ((strncmp((const char *)vdp->id, ISO_STANDARD_ID,
                    sizeof(vdp->id) + 1)) == 0) {
                return NULL;
        }

        /* We are interested in the Primary Volume Descriptor, which
         * points us to the root directory and path tables, which both
         * allow us to find any file on the CD */
        pdp = (struct iso_primary_descriptor *)vdp;

        if ((mnt = (iso9660_mnt_t *)malloc(sizeof(iso9660_mnt_t))) == NULL)
                return NULL;

        mnt->logical_block_size = isonum_723(pdp->logical_block_size);
        rootp = (struct root_directory_record *)pdp->root_directory_record;
        if ((mnt->root = (iso9660_dirent_t *)malloc(sizeof(iso9660_dirent_t))) == NULL)
                return NULL;
        memcpy(mnt->root, rootp, sizeof(mnt->root));

        return mnt;
}

static iso9660_dirent_t *
iso9660_find(iso9660_mnt_t *mnt, const char *fn, bool directory)
{
        const char *fn_cur;
        iso9660_dirent_t *xp;
        size_t fn_len;

        fn_cur = NULL;
        xp = mnt->root;

        /* Traverse directories */
        while ((fn_cur = strchr(fn, '/'))) {
                if (fn_cur != fn) {
                        fn_len = fn_cur - fn;
                        xp = iso9660_find_object(mnt, fn, fn_len, /* directory = */ true, xp);
                        if (xp == NULL)
                                return NULL;
                }
                fn = fn_cur + 1;
        }

        /* Locate the file under the resultant directory */
        if (*fn != '\0') {
                fn_len = strlen(fn);
                xp = iso9660_find_object(mnt, fn, fn_len, directory, xp);
                return xp;
        }

        return NULL;
}

static iso9660_dirent_t *
iso9660_find_object(iso9660_mnt_t *mnt, const char *fn, size_t fn_len, bool directory,
    iso9660_dirent_t *xp)
{
        char *dirent_fn;
        uint8_t flags;

        int32_t sect;
        int32_t sect_left;
        uint32_t extent;

        xp = bread(isonum_733(xp->extent));
        extent = isonum_733(xp->extent);
        sect_left = isonum_733(xp->size) / mnt->logical_block_size;
        for (sect = 0; sect < sect_left; sect++) {
                xp = bread(extent + sect);
                do {
                        flags = isonum_711(xp->flags);
                        dirent_fn = &xp->name[0];
                        if ((strncmp(dirent_fn, fn, fn_len)) == 0) {
                                if (directory && ((flags & FLAGS_DIRECTORY) == FLAGS_DIRECTORY))
                                        return xp;

                                if (!directory && ((flags & FLAGS_DIRECTORY) == 0))
                                        return xp;

                                return NULL;
                        }

                        /* Find the next directory entry */
                        xp = (iso9660_dirent_t *)((void *)xp + isonum_711(xp->length));
                } while (isonum_711(xp->length) > 0);
        }

        return NULL;
}

static void *
bread(uint16_t sector __unused)
{
        return NULL;
}
