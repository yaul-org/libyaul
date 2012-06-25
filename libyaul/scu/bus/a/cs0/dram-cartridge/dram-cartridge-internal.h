/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#ifndef _DRAM_CARTRIDGE_INTERNAL_H_
#define _DRAM_CARTRIDGE_INTERNAL_H_

#include <scu-internal.h>

#define ID              0x00FFFFFF
#define UNKNOWN         0x007EFFFC

/* Specific macros */
#define DRAM0(b, x)     (CS0((x) + (((b) & 0x03) << 19)))
#define DRAM1(b, x)     (CS0((x) + (((b) & 0x03) << 19) + 0x00200000))

extern uint8_t id;
extern void *base;

#endif /* _DRAM_CARTRIDGE_INTERNAL_H_ */
