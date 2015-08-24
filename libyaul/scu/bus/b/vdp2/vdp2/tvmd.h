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
 *          +-----------------+ <--- scanline #0 (active display,
 * VBLANK=0 | Active display  |                   start of V-BLANK-OUT (scan))
 *          +-----------------+ <--- scanline #223
 * VBLANK=1 | Bottom border   | <--- scanline #224 (bottom border,
 *          +-----------------+                     start of V-BLANK-IN (retrace))
 * VBLANK=1 | Bottom blanking |
 *          +-----------------+
 * VBLANK=1 | Vertical sync   |
 *          +-----------------+
 */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

static __inline bool
vdp2_tvmd_vblank_in(void)
{
        return (MEMORY_READ(16, VDP2(TVSTAT)) & 0x0008) == 0x0008;
}

static __inline bool
vdp2_tvmd_vblank_out(void)
{
        return (MEMORY_READ(16, VDP2(TVSTAT)) & 0x0008) == 0x0000;
}

static __inline void
vdp2_tvmd_vblank_in_wait(void)
{
        /* Wait until we're in V-BLANK-IN */
        while (vdp2_tvmd_vblank_out());
        /* Start of V-BLANK-IN */
}

static __inline void
vdp2_tvmd_vblank_out_wait(void)
{
        /* Wait until we're in V-BLANK-OUT */
        for (; vdp2_tvmd_vblank_in(); );
        /* Start of V-BLANK-OUT */
}

static __inline uint16_t
vdp2_tvmd_hcount_get(void)
{
        for (; ((MEMORY_READ(16, VDP2(EXTEN)) & 0x0200) == 0x0200); );
        return MEMORY_READ(16, VDP2(HCNT)) & 0x03FF;
}

static __inline uint16_t
vdp2_tvmd_vcount_get(void)
{
        MEMORY_WRITE(16, VDP2(EXTEN), MEMORY_READ(16, VDP2(EXTEN)) & ~0x0200);
        for (; ((MEMORY_READ(16, VDP2(TVSTAT)) & 0x0200) == 0x0200); );
        return MEMORY_READ(16, VDP2(VCNT)) & 0x03FF;
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
