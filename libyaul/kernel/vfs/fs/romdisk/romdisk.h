/*
 * Copyright (c) 2001-2003, 2012-2019
 * See LICENSE for details.
 *
 * Dan Potter
 * Lawrence Sebald
 */

#ifndef _ROMDISK_H_
#define _ROMDISK_H_

#include <errno.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <sys/queue.h>
#include <sys/types.h>

__BEGIN_DECLS

struct rd_file_handle;

typedef TAILQ_HEAD(rd_file, rd_file_handle) rd_file_t;

typedef struct rd_file_handle rd_file_handle_t;

struct rd_file_handle {
        rd_file_t *rdh;
        uint32_t index;         /* ROMFS image index */
        bool dir;               /* If a directory */
        int32_t ptr;            /* Current read position in bytes */
        size_t len;             /* Length of file in bytes */
        void *mnt;              /* Which mount instance are we using? */

        TAILQ_ENTRY(rd_file_handle) handles;
};

void romdisk_init(void);
void *romdisk_mount(const char *, const uint8_t *);
void *romdisk_open(void *, const char *);
void romdisk_close(void *);
ssize_t romdisk_read(void *, void *, size_t);
void *romdisk_direct(void *);
off_t romdisk_seek(void *, off_t, int);
off_t romdisk_tell(void *);
size_t romdisk_total(void *);

__END_DECLS

#endif /* _ROMDISK_H_ */
