/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_VDP2_SCRN_SHARED_H_
#define _YAUL_VDP2_SCRN_SHARED_H_

#include <sys/cdefs.h>

#include <stdint.h>
#include <math.h>

#include <vdp2/vram.h>

__BEGIN_DECLS

typedef enum vdp2_scrn {
        /// Normal background.
        VDP2_SCRN_NBG0    = 1 << 0,
        /// Normal background.
        VDP2_SCRN_NBG1    = 1 << 1,
        /// Normal background.
        VDP2_SCRN_NBG2    = 1 << 2,
        /// Normal background.
        VDP2_SCRN_NBG3    = 1 << 3,
        /// Rotational background.
        VDP2_SCRN_RBG0    = 1 << 4,
        /// Rotational background Parameter A.
        VDP2_SCRN_RBG0_PA = 1 << 5,
        /// Rotational background Parameter B.
        VDP2_SCRN_RBG0_PB = 1 << 6,
        /// Sprite layer.
        VDP2_SCRN_SPRITE  = 1 << 7,
        /// Back screen.
        VDP2_SCRN_BACK    = 1 << 8
} __packed vdp2_scrn_t;

typedef enum vdp2_scrn_disp {
        /// Normal background.
        VDP2_SCRN_NBG0_DISP   = 1 << 0,
        /// Normal background.
        VDP2_SCRN_NBG1_DISP   = 1 << 1,
        /// Normal background.
        VDP2_SCRN_NBG2_DISP   = 1 << 2,
        /// Normal background.
        VDP2_SCRN_NBG3_DISP   = 1 << 3,
        /// Rotational background.
        VDP2_SCRN_RBG0_DISP   = 1 << 4,
        /// Rotational background.
        VDP2_SCRN_RBG1_DISP   = 1 << 5,

        /// Normal background.
        VDP2_SCRN_NBG0_TPDISP = VDP2_SCRN_NBG0_DISP | (1 << 8),
        /// Normal background.
        VDP2_SCRN_NBG1_TPDISP = VDP2_SCRN_NBG1_DISP | (1 << 9),
        /// Normal background.
        VDP2_SCRN_NBG2_TPDISP = VDP2_SCRN_NBG2_DISP | (1 << 10),
        /// Normal background.
        VDP2_SCRN_NBG3_TPDISP = VDP2_SCRN_NBG3_DISP | (1 << 11),
        /// Rotational background.
        VDP2_SCRN_RBG0_TPDISP = VDP2_SCRN_RBG0_DISP | (1 << 12),
        /// Rotational background.
        VDP2_SCRN_RBG1_TPDISP = VDP2_SCRN_RBG1_DISP | (1 << 8)
} vdp2_scrn_disp_t;

typedef enum vdp2_scrn_ccc {
        VDP2_SCRN_CCC_PALETTE_16   = 0,
        VDP2_SCRN_CCC_PALETTE_256  = 1,
        VDP2_SCRN_CCC_PALETTE_2048 = 2,
        VDP2_SCRN_CCC_RGB_32768    = 3,
        VDP2_SCRN_CCC_RGB_16770000 = 4
} __packed vdp2_scrn_ccc_t;

extern void vdp2_scrn_back_color_set(vdp2_vram_t vram, rgb1555_t color);
extern void vdp2_scrn_back_buffer_set(vdp2_vram_t vram, const rgb1555_t *buffer, uint32_t count);
extern void vdp2_scrn_back_sync(void);

extern void vdp2_scrn_display_set(vdp2_scrn_disp_t disp_mask);
extern void vdp2_scrn_display_unset(vdp2_scrn_disp_t disp_mask);
extern void vdp2_scrn_display_clear(void);

extern void vdp2_scrn_priority_set(vdp2_scrn_t scroll_screen, uint8_t priority);
extern uint8_t vdp2_scrn_priority_get(vdp2_scrn_t scroll_screen);

extern void vdp2_scrn_scroll_x_set(vdp2_scrn_t scroll_screen, fix16_t scroll);
extern void vdp2_scrn_scroll_x_update(vdp2_scrn_t scroll_screen, fix16_t delta);
extern void vdp2_scrn_scroll_y_set(vdp2_scrn_t scroll_screen, fix16_t scroll);
extern void vdp2_scrn_scroll_y_update(vdp2_scrn_t scroll_screen, fix16_t delta);

__END_DECLS

#endif /* !_YAUL_VDP2_SCRN_SHARED_H_ */
