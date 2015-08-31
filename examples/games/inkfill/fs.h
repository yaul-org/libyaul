/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef FS_H
#define FS_H

void fs_init(void);
void *fs_open(const char *);
void fs_load(void *, void *);
size_t fs_size(void *);

#endif /* !FS_H */
