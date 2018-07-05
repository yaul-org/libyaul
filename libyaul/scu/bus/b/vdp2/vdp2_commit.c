/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2/vram.h>

#include "vdp2-internal.h"

void
vdp2_commit(void)
{
        MEMORY_WRITE(16, vdp2_state.back.vram, vdp2_state.back.color);

        uint32_t reg_count;
        reg_count = sizeof(vdp2_state.buffered_regs) / 2;

        /* Skip the first 8 VDP2 registers */
        uint32_t reg;
        for (reg = 8; reg < reg_count; reg++) {
                uint16_t offset;
                offset = reg << 1;

                MEMORY_WRITE(16, VDP2(offset), vdp2_state.buffered_regs.buffer[reg]);
        }
}
