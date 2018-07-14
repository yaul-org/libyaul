/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _ISO9660_H_
#define _ISO9660_H_

#include <sys/types.h>

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void *iso9660_mount(const char *);
void *iso9660_open(void *, const char *, int);
void iso9660_close(void *);
ssize_t iso9660_read(void *, void *, size_t);
off_t iso9660_seek(void *, off_t, int);
off_t iso9660_tell(void *);
size_t iso9660_total(void *);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ISO9660_H_ */
