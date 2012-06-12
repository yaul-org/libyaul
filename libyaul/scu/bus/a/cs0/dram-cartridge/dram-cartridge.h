/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#ifndef _DRAM_CARTRIDGE_H__
#define _DRAM_CARTRIDGE_H__

#include <inttypes.h>
#include <stddef.h>

extern size_t dram_cartridge_size(void);
extern void *dram_cartridge_area(void);
extern void dram_cartridge_init(void);

#endif /* !_DRAM_CARTRIDGE_H_ */
