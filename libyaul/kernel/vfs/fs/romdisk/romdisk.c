/*
 * Copyright (c) 2001-2003, 2012-2019
 * See LICENSE for details.
 *
 * Dan Potter
 * Lawrence Sebald
 */

#include <sys/types.h>

#include <stdlib.h>
#include <errno.h>

#include "romdisk.h"

#include <internal.h>

#define ROMFS_MAGIC             "-rom1fs-"
#define MAX_RD_FILES            16

#define TYPE_HARD_LINK          0
#define TYPE_DIRECTORY          1
#define TYPE_REGULAR_FILE       2
#define TYPE_SYMBOLIC_LINK      3
#define TYPE_BLOCK_DEVICE       4
#define TYPE_CHAR_DEVICE        5

#define IS_TYPE(x)      ((x) & 0x03)
#define TYPE_GET(x)     ((x) & 0x0F)

#define HEADER

struct rd_file_handle;

TAILQ_HEAD(rd_file, rd_file_handle);

struct rd_file_handle {
        struct rd_file *rdh;
        uint32_t index; /* ROMFS image index */
        bool dir;       /* If a directory */
        int32_t ptr;    /* Current read position in bytes */
        size_t len;     /* Length of file in bytes */
        void *mnt;      /* Which mount instance are we using? */

        TAILQ_ENTRY(rd_file_handle) handles;
};

struct romdisk_hdr {
        char magic[8];        /* Should be "-rom1fs-" */
        uint32_t full_size;   /* Full size of the file system */
        uint32_t checksum;    /* Checksum */
        char volume_name[16]; /* Volume name (zero-terminated) */
};

/* File header info; note that this header plus filename must be a multiple of
 * 16 bytes, and the following file data must also be a multiple of 16 bytes */
struct romdisk_file {
        uint32_t next_header; /* Offset of next header */
        uint32_t spec_info;   /* Spec info */
        uint32_t size;        /* Data size */
        uint32_t checksum;    /* File checksum */
        char filename[16];    /* File name (zero-terminated) */
};

struct rd_image {
        const uint8_t *image; /* The actual image */
        const struct romdisk_hdr *hdr; /* Pointer to the header */
        uint32_t files; /* Offset in the image to the files area */
};

static uint32_t _romdisk_find(struct rd_image *mnt, const char *filename, bool directory);
static uint32_t _romdisk_find_object(struct rd_image *mnt, const char *filename,
    size_t filename_len, bool directory, uint32_t offset);

/* XXX: Provisional */
static struct rd_file_handle *_romdisk_fd_alloc(void);
static void _romdisk_fd_free(struct rd_file_handle *);
/* XXX: Provisional */
static struct rd_file _fhs;

void
romdisk_init(void)
{
        TAILQ_INIT(&_fhs);
}

void *
romdisk_mount(const uint8_t *image)
{
        if (strncmp((char *)image, ROMFS_MAGIC, 8)) {
                return NULL;
        }

        struct romdisk_hdr * const hdr = (struct romdisk_hdr *)image;

        struct rd_image *mnt;
        if ((mnt = (struct rd_image *)_internal_malloc(sizeof(struct rd_image))) == NULL) {
                return NULL;
        }

        mnt->image = image;
        mnt->hdr = hdr;
        mnt->files = sizeof(struct romdisk_hdr) +
            ((strlen(hdr->volume_name) >> 4) << 4);

        return mnt;
}

void *
romdisk_open(void *fh, const char *filename)
{
        struct rd_image * const mnt = (struct rd_image *)fh;

        const bool directory = false;

        uint32_t f_idx;
        if ((f_idx = _romdisk_find(mnt, filename, directory)) == 0) {
                /* errno = ENOENT; */
                return NULL;
        }

        const struct romdisk_file * const f_hdr =
            (const struct romdisk_file *)(mnt->image + f_idx);

        struct rd_file_handle *internal_fh;
        if ((internal_fh = _romdisk_fd_alloc()) == NULL) {
                return NULL;
        }

        internal_fh->rdh = &_fhs;
        internal_fh->index = f_idx + mnt->files;
        internal_fh->dir = directory;
        internal_fh->ptr = 0;
        internal_fh->len = f_hdr->size;
        internal_fh->mnt = mnt;

        return internal_fh;
}

void
romdisk_close(void *fh)
{
        _romdisk_fd_free(fh);
}

ssize_t
romdisk_read(void *fh, void *buffer, size_t len)
{
        struct rd_file_handle *internal_fh;
        internal_fh = (struct rd_file_handle *)fh;

        struct rd_image *mnt;
        mnt = (struct rd_image *)internal_fh->mnt;

        uint8_t *ofs;

        /* Sanity checks */
        if ((internal_fh == NULL) || (internal_fh->index == 0)) {
                /* Not a valid file descriptor or is not open for
                 * reading */
                /* errno = EBADF; */
                return -1;
        }

        if (internal_fh->dir) {
                /* errno = EISDIR; */
                return -1;
        }

        /* Is there enough left? */
        if ((internal_fh->ptr + len) > internal_fh->len) {
                len = internal_fh->len - internal_fh->ptr;
        }

        ofs = (uint8_t *)(mnt->image + internal_fh->index + internal_fh->ptr);
        memcpy(buffer, ofs, len);
        internal_fh->ptr += len;

        return len;
}

void *
romdisk_direct(void *fh)
{
        struct rd_file_handle *internal_fh;
        internal_fh = (struct rd_file_handle *)fh;

        struct rd_image *mnt;
        mnt = (struct rd_image *)internal_fh->mnt;

        /* Sanity checks */
        if ((internal_fh == NULL) || (internal_fh->index == 0)) {
                /* Not a valid file descriptor or is not open for
                 * reading */
                /* errno = EBADF; */
                return NULL;
        }

        if (internal_fh->dir) {
                /* errno = EISDIR; */
                return NULL;
        }

        return (void *)(mnt->image + internal_fh->index);
}

off_t
romdisk_seek(void *fh, off_t offset, int whence)
{
        struct rd_file_handle *internal_fh;

        internal_fh = (struct rd_file_handle *)fh;

        /* Sanity checks */
        if ((internal_fh == NULL) || (internal_fh->index == 0)) {
                /* Not a valid file descriptor or is not open for
                 * reading */
                /* errno = EBADF; */
                return -1;
        }

        if (internal_fh->dir) {
                /* errno = EISDIR; */
                return -1;
        }

        /* Update current position according to arguments */
        switch(whence) {
        case SEEK_SET:
                internal_fh->ptr = offset;
                break;
        case SEEK_CUR:
                internal_fh->ptr += offset;
                break;
        case SEEK_END:
                internal_fh->ptr = internal_fh->len + offset;
                break;
        default:
                /* The whence argument to fseek() was not 'SEEK_SET',
                 * 'SEEK_END', or 'SEEK_CUR' */
                /* errno = EINVAL; */
                return -1;
        }

        /* Check bounds */
        if (internal_fh->ptr < 0) {
                internal_fh->ptr = 0;
        }

        if (internal_fh->ptr > (int32_t)internal_fh->len) {
                internal_fh->ptr = internal_fh->len;
        }

        return internal_fh->ptr;
}

off_t
romdisk_tell(void *fh)
{
        struct rd_file_handle *internal_fh;

        internal_fh = (struct rd_file_handle *)fh;
        /* Sanity checks */
        if ((internal_fh == NULL) || (internal_fh->index == 0)) {
                /* Not a valid file descriptor or is not open for
                 * reading */
                /* errno = EBADF; */
                return -1;
        }

        if (internal_fh->dir) {
                /* errno = EISDIR; */
                return -1;
        }

        return internal_fh->ptr;
}

size_t
romdisk_total(void *fh)
{
        struct rd_file_handle *internal_fh;

        internal_fh = (struct rd_file_handle *)fh;

        /* Sanity checks */
        if ((internal_fh == NULL) || (internal_fh->index == 0)) {
                /* Not a valid file descriptor or is not open for
                 * reading */
                /* errno = EBADF; */
                return -1;
        }

        if (internal_fh->dir) {
                /* errno = EISDIR; */
                return -1;
        }

        return internal_fh->len;
}

/* Given a file name and a starting ROMDISK directory listing (byte offset),
 * search for the entry in the directory and return the byte offset to its
 * entry */
static uint32_t
_romdisk_find_object(struct rd_image *mnt, const char *filename,
    size_t filename_len, bool directory, uint32_t offset)
{
        uint32_t next_ofs;
        uint32_t type;

        const struct romdisk_file *f_hdr;

        do {
                f_hdr = (const struct romdisk_file *)(mnt->image + offset);
                next_ofs = f_hdr->next_header;

                /* Since the file headers begin always at a 16 byte boundary,
                 * the lowest 4 bits would be always zero in the next filehdr
                 * pointer
                 *
                 * These four bits are used for the mode information */

                /* Bits 0..2 specify the type of the file; while bit 4 shows if
                 * the file is executable or not */
                type = TYPE_GET(next_ofs);
                next_ofs &= 0xFFFFFFF0;

                /* Check filename */
                if ((strlen(f_hdr->filename) == filename_len) &&
                    ((strncmp(f_hdr->filename, filename, filename_len)) == 0)) {
                        if (directory && (IS_TYPE(type) == TYPE_DIRECTORY)) {
                                return offset;
                        }

                        if (!directory && (IS_TYPE(type) == TYPE_REGULAR_FILE)) {
                                return offset;
                        }

                        return 0;
                }

                /* Not found */
                offset = next_ofs;
        } while (offset);

        return 0;
}

static uint32_t
_romdisk_find(struct rd_image *mnt, const char *filename, bool directory)
{
        int fn_len;

        const char *current_filename;
        current_filename = NULL;

        uint32_t ofs;
        ofs = mnt->files;

        /* Traverse directories */
        while ((current_filename = strchr(filename, '/'))) {
                if (current_filename != filename) {
                        fn_len = current_filename - filename;

                        ofs = _romdisk_find_object(mnt, filename, fn_len, /* directory = */ true, ofs);
                        if (ofs == 0) {
                                return 0;
                        }

                        const struct romdisk_file * const f_hdr =
                            (const struct romdisk_file *)(mnt->image + ofs);

                        ofs = f_hdr->spec_info;
                }

                filename = current_filename + 1;
        }

        /* Locate the file under the resultant directory */
        if (*filename != '\0') {
                fn_len = strlen(filename);
                ofs = _romdisk_find_object(mnt, filename, fn_len, directory, ofs);
                return ofs;
        }

        return 0;
}

static struct rd_file_handle *
_romdisk_fd_alloc(void)
{
        struct rd_file_handle * const fh =
            (struct rd_file_handle *)_internal_malloc(sizeof(struct rd_file_handle));

        if (fh == NULL) {
                return NULL;
        }

        TAILQ_INSERT_TAIL(&_fhs, fh, handles);

        return fh;
}

static void
_romdisk_fd_free(struct rd_file_handle *fd)
{
        if (fd == NULL) {
                return;
        }

        struct rd_file_handle *fh;

        bool fd_match;
        fd_match = false;

        TAILQ_FOREACH (fh, &_fhs, handles) {
                if (fh == fd) {
                        fd_match = true;
                        break;
                }
        }

        if (!fd_match) {
                return;
        }

        TAILQ_REMOVE(&_fhs, fh, handles);

        _internal_free(fh);
}
