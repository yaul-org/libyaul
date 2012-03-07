/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <vdp2.h>

#include <string.h>

#include "vdp2_internal.h"

void
vdp2_init(void)
{
        int i;

        /* Initialize the processor to sane values. */
	MEM_POKE(VDP2(TVMD), 0x0000);

	/* RAM */
	MEM_POKE(VDP2(RAMCTL), 0x0000);
	MEM_POKE(VDP2(CYCA0L), 0xffff);
	MEM_POKE(VDP2(CYCA0U), 0xffff);
	MEM_POKE(VDP2(CYCA1L), 0xffff);
	MEM_POKE(VDP2(CYCA1U), 0xffff);
	MEM_POKE(VDP2(CYCB0L), 0xffff);
	MEM_POKE(VDP2(CYCB0U), 0xffff);
	MEM_POKE(VDP2(CYCB1L), 0xffff);
	MEM_POKE(VDP2(CYCB1U), 0xffff);

	/* Scroll Screen */
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

	for (i = 0; i < 0x08; i++)
		MEM_POKE(VDP2(MPABN0 + i), 0x0000);

        for (i = 0; i < 0x10; i++) {
                MEM_POKE(VDP2(MPABRA + i), 0x0000);
                MEM_POKE(VDP2(MPABRB + i), 0x0000);
        }

        /* Rotation Scroll Screen */
        MEM_POKE(VDP2(RPMD), 0x0000);

	/* Line Screen */
	/* Back Screen */
        MEM_POKE(VDP2(BKTAL), 0x0000);
        MEM_POKE(VDP2(BKTAU), 0x0000);

	/* Sprite Data */
	MEM_POKE(VDP2(SPCTL), 0x0000);
        MEM_POKE(VDP2(PRISA), 0x0101);
        MEM_POKE(VDP2(PRISB), 0x0101);
        MEM_POKE(VDP2(PRISC), 0x0101);
        MEM_POKE(VDP2(PRISD), 0x0101);

        /* Priority Function */
        MEM_POKE(VDP2(PRINA), 0x0101);
        MEM_POKE(VDP2(PRINB), 0x0101);
        MEM_POKE(VDP2(PRIR), 0x0001);

	/* Color Offset Function */
        MEM_POKE(VDP2(CLOFEN), 0x0000);
        MEM_POKE(VDP2(CLOFSL), 0x0000);

        /* Reset all buffer registers. */
        memset(&vdp2_regs, 0, sizeof(struct vdp2_regs));
}
