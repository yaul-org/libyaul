/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _TVMD_H_
#define _TVMD_H_

#include <stdbool.h>
#include <inttypes.h>

#include <irq-mux.h>
#include <vdp2/map.h>

/*
 *          +-----------------+
 * VBLANK=1 | Top blanking    |
 *          +-----------------+
 * VBLANK=1 | Top border      | <--- scanline #262 (top border)
 *          +-----------------+ <--- scanline #0 (active display, V-BLANK-IN (scan))
 * VBLANK=0 | Active display  |
 *          +-----------------+ <--- scanline #223
 * VBLANK=1 | Bottom border   | <--- scanline #224 (bottom border, V-BLANK-OUT (retrace))
 *          +-----------------+
 * VBLANK=1 | Bottom blanking |
 *          +-----------------+
 * VBLANK=1 | Vertical sync   |
 *          +-----------------+
 */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

static __inline__ void
vdp2_tvmd_vblank_in_wait(void)
{
        for (; (MEMORY_READ(16, VDP2(TVSTAT)) & 0x0008) == 0x0000; );
        /* Start of V-BLANK-IN */
}

static __inline__ void
vdp2_tvmd_vblank_out_wait(void)
{
        for (; (MEMORY_READ(16, VDP2(TVSTAT)) & 0x0008) == 0x0008; );
        /* Start of V-BLANK-OUT */
}

static __inline__ uint16_t
vdp2_tvmd_hcount_get(void)
{
        for (; ((MEMORY_READ(16, VDP2(EXTEN)) & 0x0200) == 0x0200); );
        return MEMORY_READ(16, VDP2(HCNT));
}

static __inline__ uint16_t
vdp2_tvmd_vcount_get(void)
{
        for (; ((MEMORY_READ(16, VDP2(EXTEN)) & 0x0200) == 0x0200); );
        return MEMORY_READ(16, VDP2(VCNT));
}

extern irq_mux_t *vdp2_tvmd_hblank_in_irq_get(void);
extern irq_mux_t *vdp2_tvmd_vblank_in_irq_get(void);
extern irq_mux_t *vdp2_tvmd_vblank_out_irq_get(void);
extern void vdp2_tvmd_display_clear(void);
extern void vdp2_tvmd_display_set(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* !_TVMD_H_ */
