/*
 * Copyright (c) 2001-2003, 2012-2019
 * See LICENSE for details.
 *
 * Dan Potter
 * Lawrence Sebald
 */

#ifndef _YAUL_KERNEL_FS_ROMDISK_H_
#define _YAUL_KERNEL_FS_ROMDISK_H_

#include <errno.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <sys/queue.h>
#include <sys/types.h>

__BEGIN_DECLS

void romdisk_init(void);
void *romdisk_mount(const uint8_t *image);
void *romdisk_open(void *fh, const char *filename);
void romdisk_close(void *fh);
ssize_t romdisk_read(void *fh, void *buffer, size_t len);
void *romdisk_direct(void *fh);
off_t romdisk_seek(void *fh, off_t offset, int whence);
off_t romdisk_tell(void *fh);
size_t romdisk_total(void *fh);

__END_DECLS

#endif /* _YAUL_KERNEL_FS_ROMDISK_H_ */
