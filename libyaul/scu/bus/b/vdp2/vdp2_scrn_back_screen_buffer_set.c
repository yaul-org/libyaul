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
vdp2_scrn_back_screen_buffer_set(uint32_t vram, const color_rgb555_t *buffer, uint16_t count)
{
        assert(vram != 0x00000000);

        assert(buffer != NULL);

        assert(count > 0);

        _internal_state_vdp2.back.buffer = (uint16_t *)buffer;
        _internal_state_vdp2.back.count = count;

        /* Set back screen address */
        vdp2_scrn_back_screen_addr_set(vram, /* single_color = */ false);
}
