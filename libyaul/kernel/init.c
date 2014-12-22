/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include "mm/slob.h"

/**
 * Initialize the kernel.
 */
void
init(void)
{
        slob_init();
}
