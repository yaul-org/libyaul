/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>

#include <vdp2/tvmd.h>
#include <vdp2/vram.h>

#include "vdp2-internal.h"

void
vdp2_scrn_back_screen_color_set(uint32_t vram, uint16_t color)
{
        /* Write color */
        vdp2_tvmd_vblank_in_wait();
        *(volatile uint16_t *)vram = 0x8000 | color;

        /* Set back screen address */
        vdp2_scrn_back_screen_addr_set(/* single_color = */ true, vram);
}
