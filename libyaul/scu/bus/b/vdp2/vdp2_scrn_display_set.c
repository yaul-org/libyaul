/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <vdp2/scrn.h>

#include "vdp2_internal.h"

void
vdp2_scrn_display_set(enum scrn_type scrn_ena)
{
        static unsigned short bgon = 0;

        /* Enable and disable scroll screens. */
        bgon |= 1 << scrn_ena;

        /* Write to register. */
        MEM_POKE(VDP2(BGON), bgon);
}
