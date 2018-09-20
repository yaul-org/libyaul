/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _VDP2_TVMD_H_
#define _VDP2_TVMD_H_

#include <stdbool.h>
#include <stdint.h>

#include <sys/irq-mux.h>
#include <scu/map.h>
#include <vdp2/map.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*          +-----------------+
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
 *          +-----------------+ */

#define TVMD_TV_STANDARD_NTSC   0
#define TVMD_TV_STANDARD_PAL    1

#define TVMD_INTERLACE_NONE     0
#define TVMD_INTERLACE_SINGLE   1
#define TVMD_INTERLACE_DOUBLE   2

#define TVMD_VERT_224           0
#define TVMD_VERT_240           1
#define TVMD_VERT_256           2

#define TVMD_HORZ_NORMAL_A      0 /* 320x */
#define TVMD_HORZ_NORMAL_B      1 /* 352x */
#define TVMD_HORZ_HIRESO_A      2 /* 640x */
#define TVMD_HORZ_HIRESO_B      3 /* 704x */
#define TVMD_HORZ_NORMAL_AE     4 /* 320x480 */
#define TVMD_HORZ_NORMAL_BE     5 /* 352x480 */
#define TVMD_HORZ_HIRESO_AE     6 /* 640x480 */
#define TVMD_HORZ_HIRESO_BE     7 /* 704x480 */

static inline bool __attribute__ ((always_inline))
vdp2_tvmd_vblank_in(void)
{
        return (MEMORY_READ(16, VDP2(TVSTAT)) & 0x0008) == 0x0008;
}

static inline bool __attribute__ ((always_inline))
vdp2_tvmd_vblank_out(void)
{
        return (MEMORY_READ(16, VDP2(TVSTAT)) & 0x0008) == 0x0000;
}

static inline void __attribute__ ((always_inline))
vdp2_tvmd_vblank_in_wait(void)
{
        /* Wait until we're in V-BLANK-IN */
        while (vdp2_tvmd_vblank_out());
        /* Start of V-BLANK-IN */
}

static inline void __attribute__ ((always_inline))
vdp2_tvmd_vblank_out_wait(void)
{
        /* Wait until we're in V-BLANK-OUT */
        for (; vdp2_tvmd_vblank_in(); );
        /* Start of V-BLANK-OUT */
}

static inline void __attribute__ ((always_inline))
vdp2_tvmd_extern_wait(void)
{
        for (; ((MEMORY_READ(16, VDP2(EXTEN)) & 0x0200) == 0x0200); );
}

static inline void __attribute__ ((always_inline))
vdp2_tvmd_extern_latch(void)
{
        MEMORY_WRITE_AND(16, VDP2(EXTEN), ~0x0200);

        for (; ((MEMORY_READ(16, VDP2(TVSTAT)) & 0x0200) == 0x0200); );
}

static inline uint16_t __attribute__ ((always_inline))
vdp2_tvmd_hcount_get(void)
{
        return MEMORY_READ(16, VDP2(HCNT)) & 0x03FF;
}

static inline uint16_t __attribute__ ((always_inline))
vdp2_tvmd_vcount_get(void)
{
        return MEMORY_READ(16, VDP2(VCNT)) & 0x03FF;
}

static inline uint16_t __attribute__ ((always_inline))
vdp2_tvmd_tv_standard_get(void)
{
        return MEMORY_READ(16, VDP2(TVSTAT)) & 0x0001;
}

extern irq_mux_t *vdp2_tvmd_vblank_in_irq_get(void);
extern irq_mux_t *vdp2_tvmd_vblank_out_irq_get(void);
extern void vdp2_tvmd_display_clear(void);
extern void vdp2_tvmd_display_res_set(uint8_t, uint8_t, uint8_t);
extern void vdp2_tvmd_display_set(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !_VDP2_TVMD_H_ */
