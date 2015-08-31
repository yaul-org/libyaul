/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <yaul.h>

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

void
fs_read(void *fh, void *dst)
{
        assert(romdisk != NULL);
        assert(fh != NULL);

        size_t len;

        len = romdisk_read(fh, dst, romdisk_total(fh));
        assert(len == romdisk_total(fh));
}

size_t
fs_size(void *fh)
{
        assert(romdisk != NULL);
        assert(fh != NULL);

        return romdisk_total(fh);
}
