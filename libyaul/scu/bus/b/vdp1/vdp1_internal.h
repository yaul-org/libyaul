/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#ifndef _VDP1_INTERNAL_H_
#define _VDP1_INTERNAL_H_

#include <inttypes.h>

/* Write and read directly to specified address. */
#define MEM_POKE(x, y)  (*(volatile uint16_t *)(x) = (y))
#define MEM_READ(x)     (*(volatile uint16_t *)(x))

/* Macros specific for processor. */
#define CMD_TABLE(x, y) (0x25C00000 + ((x) << 5) + (((y) << 1) & 0x1F))
#define VRAM(x, y)      (0x25C04000 + ((x) + ((y) << 1))
#define GOURAUD(x, y)   (0x25C7D800 + ((x) << 4) + ((y) << 1))
#define CLUT(x, y)      (0x25C7E000 + ((x) << 4) + ((y) << 1))
#define VDP1(x)         (0x25D00000 + ((x) << 1))

enum vdp1_regs_type {
        TVMR,
        FBCR,
        PTMR,
        EWDR,
        EWLR,
        EWRR,
        ENDR,
        EDSR,
        LOPR,
        COPR,
        MODR
};

#endif /* !_VDP1_INTERNAL_H_ */
