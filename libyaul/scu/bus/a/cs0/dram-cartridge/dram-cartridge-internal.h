/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#ifndef _DRAM_CARTRIDGE_INTERNAL_H_
#define _DRAM_CARTRIDGE_INTERNAL_H_

#include <scu-internal.h>

/* DRAM cartridge registers */
#define ID              0x00FFFFFF
#define UNKNOWN         0x007EFFFE

/* Specific macros */
#define DRAM(t, b, x)   (CS0((x) + ((((b) & 0x03) +                           \
        (((t) & 0x01) << 2)) << 19)))

extern uint8_t id;
extern void *base;

#endif /* _DRAM_CARTRIDGE_INTERNAL_H_ */
