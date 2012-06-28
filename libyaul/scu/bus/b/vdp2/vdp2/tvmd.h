/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#ifndef _TVMD_H_
#define _TVMD_H_

#include <stdbool.h>
#include <inttypes.h>

#include <irq-mux.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern irq_mux_t *vdp2_tvmd_vblank_in_irq_get(void);
extern irq_mux_t *vdp2_tvmd_vblank_out_irq_get(void);
extern uint16_t vdp2_tvmd_hcount_get(void);
extern uint16_t vdp2_tvmd_vcount_get(void);
extern void vdp2_tvmd_blcs_set(bool, uint32_t, uint16_t *, uint16_t );
extern void vdp2_tvmd_display_clear(void);
extern void vdp2_tvmd_display_set(void);
extern void vdp2_tvmd_vblank_in_wait(void);
extern void vdp2_tvmd_vblank_out_wait(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* !_TVMD_H_ */
