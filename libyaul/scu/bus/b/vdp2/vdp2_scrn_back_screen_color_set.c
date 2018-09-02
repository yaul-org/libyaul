/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>

#include <vdp2/cram.h>
#include <vdp2/scrn.h>
#include <vdp2/tvmd.h>
#include <vdp2/vram.h>

#include "vdp2-internal.h"

void
vdp2_scrn_back_screen_color_set(uint32_t vram, color_rgb555_t color)
{
        vdp2_state.back.color = COLOR_RGB_DATA | color.raw;

        /* Set back screen address */
        vdp2_scrn_back_screen_addr_set(vram, /* single_color = */ true);
}
