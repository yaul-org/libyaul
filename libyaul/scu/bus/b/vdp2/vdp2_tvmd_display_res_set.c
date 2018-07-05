/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <bios.h>
#include <cpu.h>
#include <vdp2/tvmd.h>

#include <assert.h>

#include "vdp2-internal.h"

void
vdp2_tvmd_display_res_set(uint8_t interlace, uint8_t horizontal,
    uint8_t vertical)
{
#ifdef DEBUG
        /* Check interlace values */
        assert((interlace == TVMD_INTERLACE_NONE) ||
               (interlace == TVMD_INTERLACE_SINGLE) ||
               (interlace == TVMD_INTERLACE_DOUBLE));

        /* Check vertical resolution */
        assert((vertical == TVMD_VERT_224) ||
               (vertical == TVMD_VERT_240) ||
               (vertical == TVMD_VERT_256));

        /* Check horizontal resolution */
        assert((horizontal == TVMD_HORZ_NORMAL_A) ||
               (horizontal == TVMD_HORZ_NORMAL_B) ||
               (horizontal == TVMD_HORZ_HIRESO_A) ||
               (horizontal == TVMD_HORZ_HIRESO_B) ||
               (horizontal == TVMD_HORZ_NORMAL_AE) ||
               (horizontal == TVMD_HORZ_NORMAL_BE) ||
               (horizontal == TVMD_HORZ_HIRESO_AE) ||
               (horizontal == TVMD_HORZ_HIRESO_BE));
#endif /* DEBUG */

        uint32_t sys_clock;
        sys_clock = bios_clock_speed_get();

        uint16_t display_w;
        uint16_t display_h;

        vdp2_state.buffered_regs.tvmd &= 0xFFCF;
        switch (vertical)  {
        case TVMD_VERT_224:
                display_h = 224;
                break;
        case TVMD_VERT_240:
                display_h = 240;
                vdp2_state.buffered_regs.tvmd |= 0x0010;
                break;
        case TVMD_VERT_256:
                display_h = 256;
                vdp2_state.buffered_regs.tvmd |= 0x0020;
                break;
        default:
                return;
        }

        vdp2_state.buffered_regs.tvmd &= 0xFF3F;
        switch (interlace)  {
        case TVMD_INTERLACE_NONE:
                break;
        case TVMD_INTERLACE_SINGLE:
                vdp2_state.buffered_regs.tvmd |= 0x0080;
                break;
        case TVMD_INTERLACE_DOUBLE:
                display_h = 2 * vertical;
                vdp2_state.buffered_regs.tvmd |= 0x00C0;
                break;
        default:
                return;
        }

        uint16_t clock_freq;
        clock_freq = CPU_CLOCK_SPEED_26MHZ;

        vdp2_state.buffered_regs.tvmd &= 0xFFF0;
        switch (horizontal) {
        case TVMD_HORZ_NORMAL_A:
                display_w = 320;
                clock_freq = CPU_CLOCK_SPEED_26MHZ;
                break;
        case TVMD_HORZ_NORMAL_B:
                display_w = 352;
                clock_freq = CPU_CLOCK_SPEED_28MHZ;
                vdp2_state.buffered_regs.tvmd |= 0x0001;
                break;
        case TVMD_HORZ_HIRESO_A:
                display_w = 640;
                clock_freq = CPU_CLOCK_SPEED_26MHZ;
                vdp2_state.buffered_regs.tvmd |= 0x0002;
                break;
        case TVMD_HORZ_HIRESO_B:
                display_w = 704;
                clock_freq = CPU_CLOCK_SPEED_28MHZ;
                vdp2_state.buffered_regs.tvmd |= 0x0003;
                break;
        case TVMD_HORZ_NORMAL_AE:
                display_w = 320;
                display_h = 480;
                clock_freq = CPU_CLOCK_SPEED_26MHZ;
                vdp2_state.buffered_regs.tvmd |= 0x0004;
                break;
        case TVMD_HORZ_NORMAL_BE:
                display_w = 352;
                display_h = 480;
                clock_freq = CPU_CLOCK_SPEED_28MHZ;
                vdp2_state.buffered_regs.tvmd |= 0x0005;
                break;
        case TVMD_HORZ_HIRESO_AE:
                display_w = 640;
                display_h = 480;
                clock_freq = CPU_CLOCK_SPEED_26MHZ;
                vdp2_state.buffered_regs.tvmd |= 0x0006;
                break;
        case TVMD_HORZ_HIRESO_BE:
                display_w = 704;
                display_h = 480;
                clock_freq = CPU_CLOCK_SPEED_28MHZ;
                vdp2_state.buffered_regs.tvmd |= 0x0007;
                break;
        default:
                return;
        }

        /* If clock frequency is not set, change to the correct
         * frequency */
        if (sys_clock != clock_freq) {
                bios_clock_speed_change(clock_freq);
        }

        /* Update state */
        vdp2_state.display_w = display_w;
        vdp2_state.display_h = display_h;
        vdp2_state.interlaced = interlace;

        /* Write to TVMD bit during VBLANK-IN */
        vdp2_tvmd_vblank_in_wait();

        /* write to memory */
        MEMORY_WRITE(16, VDP2(TVMD), vdp2_state.buffered_regs.tvmd);
}
