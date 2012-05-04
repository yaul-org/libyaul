/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#ifndef _VRAM_H_
#define _VRAM_H_

#include <inttypes.h>

#define VRAM_BANK_4MBIT(x, y)   (0x25E00000 + (((x) & 0x3) << 17) + (y))
#define VRAM_BANK_8MBIT(x, y)   (0x25E00000 + (((x) & 0x3) << 18) + (y))

extern void vdp2_vram_cycle_pattern_set(uint32_t *);

#endif /* !_VRAM_H_ */
