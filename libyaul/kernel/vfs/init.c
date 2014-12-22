/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include "vfs.h"

void
vfs_init(void)
{
        pathname_cache_init();
}
