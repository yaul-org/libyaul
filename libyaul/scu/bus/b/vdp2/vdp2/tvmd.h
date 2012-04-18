/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#ifndef _TVMD_H_
#define _TVMD_H_

#include <stdbool.h>
#include <inttypes.h>

extern bool vdp2_tvmd_vblank_status_get(void);
extern void vdp2_tvmd_blcs_set(bool, uint8_t, uint32_t, uint16_t *, uint16_t);
extern void vdp2_tvmd_display_clear(void);
extern void vdp2_tvmd_display_set(void);
extern void vdp2_tvmd_vblank_in_wait(void);
extern void vdp2_tvmd_vblank_out_wait(void);

#endif
