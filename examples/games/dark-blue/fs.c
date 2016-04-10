/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <yaul.h>
#include <tga.h>

#include <assert.h>

extern uint8_t root_romdisk[];

static void *romdisk = NULL;

void
fs_init(void)
{
        romdisk_init();
        romdisk = romdisk_mount("/", root_romdisk);
        assert(romdisk != NULL);
}

void *
fs_open(const char *path)
{
        assert(romdisk != NULL);

        void *fh;

        fh = romdisk_open(romdisk, path, O_RDONLY);
        assert(fh != NULL);

        return fh;
}

void
fs_close(void *fh)
{
        assert(romdisk != NULL);
        assert(fh != NULL);

        romdisk_close(fh);
}
#include "globals.h"
void
fs_read(void *fh, void *dst, size_t len)
{
        assert(romdisk != NULL);
        assert(fh != NULL);

        size_t read_len;

        read_len = romdisk_read(fh, dst, len);
        assert(read_len == len);
}

size_t
fs_size(void *fh)
{
        assert(romdisk != NULL);
        assert(fh != NULL);

        return romdisk_total(fh);
}

off_t
fs_seek(void *fh, off_t offset, int whence)
{
        assert(romdisk != NULL);
        assert(fh != NULL);

        return romdisk_seek(fh, offset, whence);
}
