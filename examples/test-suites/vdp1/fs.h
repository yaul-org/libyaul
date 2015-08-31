/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef FS_H
#define FS_H

size_t fs_size(void *);
void *fs_open(const char *);
void fs_close(void *);
void fs_init(void);
void fs_read(void *, void *);

#endif /* !FS_H */
