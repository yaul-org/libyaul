/*
 * Copyright (c) 2001-2003, 2012
 * See LICENSE for details.
 *
 * Dan Potter
 * Lawrence Sebald
 */

#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>

#include "romdisk.h"

/* XXX
 * Only temporary solution
 */
#define __unused        __attribute__ ((unused))

typedef struct {
        char magic[8];          /* Should be "-rom1fs-" */
        uint32_t full_size;     /* Full size of the file system */
        uint32_t checksum;      /* Checksum */
        char volume_name[16];   /* Volume name (zero-terminated) */
} romdisk_hdr_t;

/* File header info; note that this header plus filename must be a
 * multiple of 16 bytes, and the following file data must also be a
 * multiple of 16 bytes */
typedef struct {
        uint32_t next_header;   /* Offset of next header */
        uint32_t spec_info;     /* Spec info */
        uint32_t size;          /* Data size */
        uint32_t checksum;      /* File checksum */
        char filename[16];      /* File name (zero-terminated) */
} __attribute__ ((aligned (16))) romdisk_file_t;

typedef struct  {
        const uint8_t *image;     /* The actual image */
        const romdisk_hdr_t *hdr; /* Pointer to the header */
        uint32_t files;      /* Offset in the image to the files area */
} rd_image_t;

static uint32_t romdisk_find_object(rd_image_t *, const char *, int, int, uint32_t);
static void *romdisk_open(const char *, int);

void
romdisk_init(void)
{
}

static void * __attribute__ ((used))
romdisk_open(const char *fn __unused, int mode __unused)
{
        return NULL;
}

static void __unused
romdisk_close(void *h __unused)
{
}

static ssize_t __unused
romdisk_read(void *h __unused, void *buf __unused, size_t bytes __unused)
{
        return 0;
}

static off_t __unused
romdisk_seek(void *h __unused, off_t offset __unused, int whence __unused)
{
        return 0;
}

static off_t __unused
romdisk_tell(void *h __unused)
{
        return 0;
}

static size_t __unused
romdisk_total(void *h __unused)
{
        return 0;
}

/* Given a filename and a starting romdisk directory listing (byte
 * offset), search for the entry in the directory and return the byte
 * offset to its entry */
static uint32_t __unused
romdisk_find_object(rd_image_t *mnt __unused,
    const char *fn __unused,
    int fnlen __unused,
    int dir __unused,
    uint32_t offset __unused)
{
        return 0;
}

static uint32_t __unused
romdisk_find(rd_image_t *mnt __unused,
    const char *fn __unused,
    int dir __unused)
{
        return 0;
}
