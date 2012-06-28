/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#ifndef _ROMDISK_H_
#define _ROMDISK_H_

#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <sys/queue.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

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
void *romdisk_open(void *, const char *, int);
void romdisk_close(void *);
ssize_t romdisk_read(void *, void *, size_t);
off_t romdisk_seek(void *, off_t, int);
off_t romdisk_tell(void *);
size_t romdisk_total(void *);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _ROMDISK_H_ */
