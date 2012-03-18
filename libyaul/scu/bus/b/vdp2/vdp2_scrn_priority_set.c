/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <vdp2/scrn.h>

#include "vdp2_internal.h"

static unsigned short prina = 0;
static unsigned short prinb = 0;

void
vdp2_scrn_priority_set(enum scrn_type scrn, unsigned char priority)
{
        /* When priority is zero, scroll screen is transparent. */
        priority &= 0x7;

        switch (scrn) {
        case SCRN_NBG0:
        case SCRN_RBG1:
                prina &= 0x0f00;
                prina |= priority;

                /* Write to memory. */
                MEM_POKE(VDP2(PRINA), prina);
                break;
        case SCRN_NBG1:
                prina &= 0x000f;
                prina |= priority << 8;

                /* Write to memory. */
                MEM_POKE(VDP2(PRINA), prina);
                break;
        case SCRN_NBG2:
                prinb &= 0x0f00;
                prinb |= priority;

                /* Write to memory. */
                MEM_POKE(VDP2(PRINB), prinb);
                break;
        case SCRN_NBG3:
                prinb &= 0x000f;
                prinb |= priority << 8;

                /* Write to memory. */
                MEM_POKE(VDP2(PRINB), prinb);
                break;
        case SCRN_RBG0:
                /* Write to memory. */
                MEM_POKE(VDP2(PRIR), priority);
                break;
        default:
                break;
        }
}
