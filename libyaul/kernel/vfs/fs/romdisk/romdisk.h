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
void *romdisk_mount(const char *, const uint8_t *);
void *romdisk_open(void *, const char *);
void romdisk_close(void *);
ssize_t romdisk_read(void *, void *, size_t);
void *romdisk_direct(void *);
off_t romdisk_seek(void *, off_t, int);
off_t romdisk_tell(void *);
size_t romdisk_total(void *);

__END_DECLS

#endif /* _YAUL_KERNEL_FS_ROMDISK_H_ */
