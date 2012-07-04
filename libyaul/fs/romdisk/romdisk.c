/*
 * Copyright (c) 2001-2003, 2012
 * See LICENSE for details.
 *
 * Dan Potter
 * Lawrence Sebald
 */

#include <stdlib.h>

#include "romdisk.h"

#define ROMFS_MAGIC             "-rom1fs-"
#define MAX_RD_FILES            16

#define TYPE_HARD_LINK          0
#define TYPE_DIRECTORY          1
#define TYPE_REGULAR_FILE       2
#define TYPE_SYMBOLIC_LINK      3
#define TYPE_BLOCK_DEVICE       4
#define TYPE_CHAR_DEVICE        5

#define IS_TYPE(x)              ((x) & 0x03)
#define TYPE_GET(x)             ((x) & 0x0F)

#define HEADER

typedef struct {
        char magic[8];          /* Should be "-rom1fs-" */
        uint32_t full_size;     /* Full size of the file system */
        uint32_t checksum;      /* Checksum */
        char volume_name[16];   /* Volume name (zero-terminated) */
}  romdisk_hdr_t;

/* File header info; note that this header plus filename must be a
 * multiple of 16 bytes, and the following file data must also be a
 * multiple of 16 bytes */
typedef struct {
        uint32_t next_header;   /* Offset of next header */
        uint32_t spec_info;     /* Spec info */
        uint32_t size;          /* Data size */
        uint32_t checksum;      /* File checksum */
        char filename[16];      /* File name (zero-terminated) */
} romdisk_file_t;

typedef struct {
        const uint8_t *image;   /* The actual image */
        const romdisk_hdr_t *hdr; /* Pointer to the header */
        uint32_t files;         /* Offset in the image to the files area */
} rd_image_t;

static uint32_t romdisk_find(rd_image_t *, const char *, bool);
static uint32_t romdisk_find_object(rd_image_t *, const char *, size_t, bool,
    uint32_t);

/* XXX Provisional */
static rd_file_handle_t *romdisk_fd_alloc(void);
static void romdisk_fd_free(rd_file_handle_t *);
/* XXX Provisional */
static rd_file_t fhs;

void
romdisk_init(void)
{

        TAILQ_INIT(&fhs);
}

void *
romdisk_mount(const char *mnt_point __attribute__ ((unused)),
    const uint8_t *image)
{
        romdisk_hdr_t *hdr;
        rd_image_t *mnt;

        if (strncmp((char *)image, ROMFS_MAGIC, 8))
                return NULL;

        hdr = (romdisk_hdr_t *)image;
        if ((mnt = (rd_image_t *)malloc(sizeof(rd_image_t))) == NULL)
                return NULL;

        mnt->image = image;
        mnt->hdr = hdr;
        mnt->files = sizeof(romdisk_hdr_t) + ((strlen(hdr->volume_name) >> 4) << 4);

        return mnt;
}

void *
romdisk_open(void *p, const char *fn,
    int mode __attribute__ ((unused)))
{
        rd_image_t *mnt;
        rd_file_handle_t *fh;

        const romdisk_file_t *f_hdr;
        uint32_t f_idx;
        bool directory;

        mnt = (rd_image_t *)p;
        directory = false;
        if ((f_idx = romdisk_find(mnt, fn, directory)) == 0) {
                errno = ENOENT;
                return NULL;
        }

        f_hdr = (const romdisk_file_t *)(mnt->image + f_idx);

        fh = romdisk_fd_alloc();
        fh->rdh = &fhs;
        fh->index = f_idx + mnt->files;
        fh->dir = directory;
        fh->ptr = 0;
        fh->len = f_hdr->size;
        fh->mnt = mnt;

        return fh;
}

void
romdisk_close(void *fh)
{
        romdisk_fd_free(fh);
}

ssize_t
romdisk_read(void *p, void *buf, size_t bytes)
{
        rd_file_handle_t *fh;
        rd_image_t *mnt;
        uint8_t *ofs;

        fh = (rd_file_handle_t *)p;
        mnt = (rd_image_t *)fh->mnt;

        /* Sanity checks */
        if ((fh == NULL) || (fh->index == 0)) {
                /* Not a valid file descriptor or is not open for
                 * reading */
                errno = EBADF;
                return -1;
        }

        if (fh->dir) {
                errno = EISDIR;
                return -1;
        }

        /* Is there enough left? */
        if ((fh->ptr + bytes) > fh->len)
                bytes = fh->len - fh->ptr;

        ofs = (uint8_t *)(mnt->image + fh->index + fh->ptr);
        memcpy(buf, ofs, bytes);
        fh->ptr += bytes;

        return bytes;
}

off_t
romdisk_seek(void *p, off_t offset, int whence)
{
        rd_file_handle_t *fh;

        fh = (rd_file_handle_t *)p;

        /* Sanity checks */
        if ((fh == NULL) || (fh->index == 0)) {
                /* Not a valid file descriptor or is not open for
                 * reading */
                errno = EBADF;
                return -1;
        }

        if (fh->dir) {
                errno = EISDIR;
                return -1;
        }

        /* Update current position according to arguments */
        switch(whence) {
        case SEEK_SET:
                fh->ptr = offset;
                break;
        case SEEK_CUR:
                fh->ptr += offset;
                break;
        case SEEK_END:
                fh->ptr = fh->len + offset;
                break;
        default:
                /* The whence argument to fseek() was not 'SEEK_SET',
                 * 'SEEK_END', or 'SEEK_CUR' */
                errno = EINVAL;
                return -1;
        }

        /* Check bounds */
        if(fh->ptr < 0)
                fh->ptr = 0;

        if(fh->ptr > (int32_t)fh->len)
                fh->ptr = fh->len;

        return fh->ptr;
}

off_t
romdisk_tell(void *p)
{
        rd_file_handle_t *fh;

        fh = (rd_file_handle_t *)p;
        /* Sanity checks */
        if ((fh == NULL) || (fh->index == 0)) {
                /* Not a valid file descriptor or is not open for
                 * reading */
                errno = EBADF;
                return -1;
        }

        if (fh->dir) {
                errno = EISDIR;
                return -1;
        }

        return fh->ptr;
}

size_t
romdisk_total(void *p)
{
        rd_file_handle_t *fh;

        fh = (rd_file_handle_t *)p;

        /* Sanity checks */
        if ((fh == NULL) || (fh->index == 0)) {
                /* Not a valid file descriptor or is not open for
                 * reading */
                errno = EBADF;
                return -1;
        }

        if (fh->dir) {
                errno = EISDIR;
                return -1;
        }

        return fh->len;
}

/* Given a file name and a starting ROMDISK directory listing (byte
 * offset), search for the entry in the directory and return the byte
 * offset to its entry */
static uint32_t
romdisk_find_object(rd_image_t *mnt, const char *fn, size_t fn_len, bool directory,
    uint32_t offset)
{
        uint32_t next_ofs;
        uint32_t type;

        const romdisk_file_t *f_hdr;

        do {
                f_hdr = (const romdisk_file_t *)(mnt->image + offset);
                next_ofs = f_hdr->next_header;

                /* Since the file headers begin always at a 16 byte
                 * boundary, the lowest 4 bits would be always zero in
                 * the next filehdr pointer
                 *
                 * These four bits are used for the mode information */

                /* Bits 0..2 specify the type of the file; while bit 4
                 * shows if the file is executable or not */
                type = TYPE_GET(next_ofs);
                next_ofs &= 0xFFFFFFF0;

                /* Check filename */
                if ((strlen(f_hdr->filename) == fn_len) &&
                    ((strncmp(f_hdr->filename, fn, fn_len)) == 0)) {
                        if (directory && (IS_TYPE(type) == TYPE_DIRECTORY))
                                return offset;

                        if (!directory && (IS_TYPE(type) == TYPE_REGULAR_FILE))
                                return offset;

                        return 0;
                }

                /* Not found */
                offset = next_ofs;
        } while (offset);

        return 0;
}

static uint32_t
romdisk_find(rd_image_t *mnt, const char *fn, bool directory)
{
        const char *fn_cur;
        uint32_t ofs;
        int fn_len;
        const romdisk_file_t *f_hdr;

        fn_cur = NULL;
        ofs = mnt->files;

        /* Traverse directories */
        while ((fn_cur = strchr(fn, '/'))) {
                if (fn_cur != fn) {
                        fn_len = fn_cur - fn;
                        if ((ofs = romdisk_find_object(mnt,
                                    fn, fn_len, /* directory = */ true, ofs)) == 0)
                                return 0;

                        f_hdr = (const romdisk_file_t *)(mnt->image + ofs);
                        ofs = f_hdr->spec_info;
                }

                fn = fn_cur + 1;
        }

        /* Locate the file under the resultant directory */
        if (*fn != '\0') {
                fn_len = strlen(fn);
                ofs = romdisk_find_object(mnt, fn, fn_len, directory, ofs);
                return ofs;
        }

        return 0;
}

static rd_file_handle_t *
romdisk_fd_alloc(void)
{
        rd_file_handle_t *fh;

        if ((fh = (rd_file_handle_t *)malloc(sizeof(rd_file_handle_t))) == NULL)
                return NULL;

        TAILQ_INSERT_TAIL(&fhs, fh, handles);

        return fh;
}

static void
romdisk_fd_free(rd_file_handle_t *fd)
{
        rd_file_handle_t *fh;

        if (fd != NULL)
                return;

        TAILQ_FOREACH(fh, &fhs, handles) {
                if (fh == fd) {
                        TAILQ_REMOVE(&fhs, fh, handles);
                        free(fh);
                        return;
                }
        }
}
