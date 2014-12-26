/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _VDP1_MAP_H_
#define _VDP1_MAP_H_

#include <scu-internal.h>

/* Macros specific for processor */
#define VDP1(x)         (0x25D00000 + (x))

/* Helpers specific to this processor */
#define CMD_TABLE(x, y) (0x25C00000 + ((x) << 5) + (((y) << 1) & 0x1F))
#define CLUT(x, y)      (0x25C00000 + VDP1_CMDT_MEMORY_SIZE + ((x) << 4) + ((y) << 1))
#define GOURAUD(x, y)   (0x25C00000 + VDP1_CMDT_MEMORY_SIZE + VDP1_CLUT_MEMORY_SIZE + ((x) << 4) + ((y) << 1))
#define VRAM(x)         (0x25C00000 + VDP1_CMDT_MEMORY_SIZE + VDP1_TEXURE_MEMORY_SIZE + VDP1_GST_MEMORY_SIZE + ((x)))
#define FRAME_BUFFER(x) (0x25C80000 + ((x)))

#define TVMR		0x0000
#define FBCR		0x0002
#define PTMR		0x0004
#define EWDR		0x0006
#define EWLR		0x0008
#define EWRR		0x000A
#define ENDR		0x000C
#define EDSR		0x000E
#define LOPR		0x0010
#define COPR		0x0012
#define MODR		0x0014

#endif /* !_VDP1_MAP_H_ */
