/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <vdp2.h>

#include <string.h>

#include "vdp2_internal.h"

struct vdp2_regs vdp2_regs;

void
vdp2_init(void)
{

        /* Initialize the processor to sane values. */
        MEM_POKE(VDP2(TVMD), 0x0000);

        /* RAM */
        /* Partition both VRAM bank A and B by default */
        MEM_POKE(VDP2(RAMCTL), 0x0300);
        MEM_POKE(VDP2(CYCA0L), 0xFFFF);
        MEM_POKE(VDP2(CYCA0U), 0xFFFF);
        MEM_POKE(VDP2(CYCA1L), 0xFFFF);
        MEM_POKE(VDP2(CYCA1U), 0xFFFF);
        MEM_POKE(VDP2(CYCB0L), 0xFFFF);
        MEM_POKE(VDP2(CYCB0U), 0xFFFF);
        MEM_POKE(VDP2(CYCB1L), 0xFFFF);
        MEM_POKE(VDP2(CYCB1U), 0xFFFF);

        /* Scroll screen */
        MEM_POKE(VDP2(BGON), 0x0000);
        MEM_POKE(VDP2(CHCTLA), 0x0000);
        MEM_POKE(VDP2(CHCTLB), 0x0000);
        MEM_POKE(VDP2(PNCN0), 0x0000);
        MEM_POKE(VDP2(PNCN1), 0x0000);
        MEM_POKE(VDP2(PNCN2), 0x0000);
        MEM_POKE(VDP2(PNCN3), 0x0000);
        MEM_POKE(VDP2(PLSZ), 0x0000);
        MEM_POKE(VDP2(BMPNA), 0x0000);
        MEM_POKE(VDP2(BMPNB), 0x0000);
        MEM_POKE(VDP2(MPOFN), 0x0000);
        MEM_POKE(VDP2(MPOFR), 0x0000);

        MEM_POKE(VDP2(MPABN0), 0x0000);
        MEM_POKE(VDP2(MPCDN0), 0x0000);
        MEM_POKE(VDP2(MPABN1), 0x0000);
        MEM_POKE(VDP2(MPCDN1), 0x0000);
        MEM_POKE(VDP2(MPABN2), 0x0000);
        MEM_POKE(VDP2(MPCDN2), 0x0000);
        MEM_POKE(VDP2(MPABN3), 0x0000);
        MEM_POKE(VDP2(MPCDN3), 0x0000);

        MEM_POKE(VDP2(SCXIN0), 0x0000);
        MEM_POKE(VDP2(SCYIN0), 0x0000);
        MEM_POKE(VDP2(SCXIN1), 0x0000);
        MEM_POKE(VDP2(SCYIN1), 0x0000);
        MEM_POKE(VDP2(SCXN2), 0x0000);
        MEM_POKE(VDP2(SCYN2), 0x0000);
        MEM_POKE(VDP2(SCXN3), 0x0000);
        MEM_POKE(VDP2(SCYN3), 0x0000);

        /* Rotation scroll screen */
        MEM_POKE(VDP2(RPMD), 0x0000);

        MEM_POKE(VDP2(MPABRA), 0x0000);
        MEM_POKE(VDP2(MPCDRA), 0x0000);
        MEM_POKE(VDP2(MPEFRA), 0x0000);
        MEM_POKE(VDP2(MPGHRA), 0x0000);
        MEM_POKE(VDP2(MPIJRA), 0x0000);
        MEM_POKE(VDP2(MPKLRA), 0x0000);
        MEM_POKE(VDP2(MPMNRA), 0x0000);
        MEM_POKE(VDP2(MPOPRA), 0x0000);
        MEM_POKE(VDP2(MPABRB), 0x0000);
        MEM_POKE(VDP2(MPCDRB), 0x0000);
        MEM_POKE(VDP2(MPEFRB), 0x0000);
        MEM_POKE(VDP2(MPGHRB), 0x0000);
        MEM_POKE(VDP2(MPIJRB), 0x0000);
        MEM_POKE(VDP2(MPKLRB), 0x0000);
        MEM_POKE(VDP2(MPMNRB), 0x0000);
        MEM_POKE(VDP2(MPOPRB), 0x0000);

        /* Line screen */
        /* Back screen */
        MEM_POKE(VDP2(BKTAL), 0x0000);
        MEM_POKE(VDP2(BKTAU), 0x0000);

        /* Sprite data */
        MEM_POKE(VDP2(SPCTL), 0x0000);
        MEM_POKE(VDP2(PRISA), 0x0101);
        MEM_POKE(VDP2(PRISB), 0x0101);
        MEM_POKE(VDP2(PRISC), 0x0101);
        MEM_POKE(VDP2(PRISD), 0x0101);

        /* Priority function */
        MEM_POKE(VDP2(PRINA), 0x0101);
        MEM_POKE(VDP2(PRINB), 0x0101);
        MEM_POKE(VDP2(PRIR), 0x0001);

        /* Color offset function */
        MEM_POKE(VDP2(CLOFEN), 0x0000);
        MEM_POKE(VDP2(CLOFSL), 0x0000);

        /* Reset all buffer registers. */
        memset(&vdp2_regs, 0x0000, sizeof(struct vdp2_regs));
}
