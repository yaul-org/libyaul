/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp1.h>

#include "vdp1-internal.h"

void
vdp1_init(void)
{
        /* Initialize the processor to sane values. */
        MEMORY_WRITE(16, VDP1(TVMR), 0x0000);
        MEMORY_WRITE(16, VDP1(ENDR), 0x0000);
        MEMORY_WRITE(16, VDP1(FBCR), 0x0000);
        MEMORY_WRITE(16, VDP1(EWLR), 0x0000);
        MEMORY_WRITE(16, VDP1(EWRR), 0x50DF);
        MEMORY_WRITE(16, VDP1(EWDR), 0x8000);
        MEMORY_WRITE(16, VDP1(PTMR), 0x0002);

        /* Stop processing command tables. */
        MEMORY_WRITE(16, CMD_TABLE(0, 0), 0x8000);
}
