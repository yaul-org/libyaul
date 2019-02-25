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

#include "vdp-internal.h"

void
vdp2_tvmd_display_clear(void)
{
        _state_vdp2()->regs.tvmd &= 0x7EFF;
}

void
vdp2_tvmd_display_set(void)
{
        _state_vdp2()->regs.tvmd |= 0x8000;
}

void
vdp2_tvmd_display_res_get(uint16_t *width, uint16_t *height)
{
        assert(width != NULL);
        assert(height != NULL);

        *width = _state_vdp2()->tv.resolution.x;
        *height = _state_vdp2()->tv.resolution.y;
}

void
vdp2_tvmd_display_res_set(uint8_t interlace, uint8_t horizontal,
    uint8_t vertical)
{
        /* Check interlace values */
        assert((interlace == VDP2_TVMD_INTERLACE_NONE) ||
               (interlace == VDP2_TVMD_INTERLACE_SINGLE) ||
               (interlace == VDP2_TVMD_INTERLACE_DOUBLE));

        /* Check vertical resolution */
        assert((vertical == VDP2_TVMD_VERT_224) ||
               (vertical == VDP2_TVMD_VERT_240) ||
               (vertical == VDP2_TVMD_VERT_256));

        /* Check horizontal resolution */
        assert((horizontal == VDP2_TVMD_HORZ_NORMAL_A) ||
               (horizontal == VDP2_TVMD_HORZ_NORMAL_B) ||
               (horizontal == VDP2_TVMD_HORZ_HIRESO_A) ||
               (horizontal == VDP2_TVMD_HORZ_HIRESO_B) ||
               (horizontal == VDP2_TVMD_HORZ_NORMAL_AE) ||
               (horizontal == VDP2_TVMD_HORZ_NORMAL_BE) ||
               (horizontal == VDP2_TVMD_HORZ_HIRESO_AE) ||
               (horizontal == VDP2_TVMD_HORZ_HIRESO_BE));

        uint32_t sys_clock;
        sys_clock = bios_clock_speed_get();

        uint16_t width;
        uint16_t height;

        _state_vdp2()->regs.tvmd &= 0xFFCF;

        switch (vertical)  {
        case VDP2_TVMD_VERT_224:
                height = 224;
                break;
        case VDP2_TVMD_VERT_240:
                height = 240;
                _state_vdp2()->regs.tvmd |= 0x0010;
                break;
        case VDP2_TVMD_VERT_256:
                height = 256;
                _state_vdp2()->regs.tvmd |= 0x0020;
                break;
        default:
                assert(false);
                return;
        }

        _state_vdp2()->regs.tvmd &= 0xFF3F;

        switch (interlace)  {
        case VDP2_TVMD_INTERLACE_NONE:
                break;
        case VDP2_TVMD_INTERLACE_SINGLE:
                _state_vdp2()->regs.tvmd |= 0x0080;
                break;
        case VDP2_TVMD_INTERLACE_DOUBLE:
                height *= 2;
                _state_vdp2()->regs.tvmd |= 0x00C0;
                break;
        default:
                assert(false);
                return;
        }

        uint16_t clock_freq;
        clock_freq = CPU_CLOCK_SPEED_26MHZ;

        _state_vdp2()->regs.tvmd &= 0xFFF0;

        switch (horizontal) {
        case VDP2_TVMD_HORZ_NORMAL_A:
                width = 320;
                clock_freq = CPU_CLOCK_SPEED_26MHZ;
                break;
        case VDP2_TVMD_HORZ_NORMAL_B:
                width = 352;
                clock_freq = CPU_CLOCK_SPEED_28MHZ;
                _state_vdp2()->regs.tvmd |= 0x0001;
                break;
        case VDP2_TVMD_HORZ_HIRESO_A:
                width = 640;
                clock_freq = CPU_CLOCK_SPEED_26MHZ;
                _state_vdp2()->regs.tvmd |= 0x0002;
                break;
        case VDP2_TVMD_HORZ_HIRESO_B:
                width = 704;
                clock_freq = CPU_CLOCK_SPEED_28MHZ;
                _state_vdp2()->regs.tvmd |= 0x0003;
                break;
        case VDP2_TVMD_HORZ_NORMAL_AE:
                width = 320;
                height = 480;
                clock_freq = CPU_CLOCK_SPEED_26MHZ;
                _state_vdp2()->regs.tvmd |= 0x0004;
                break;
        case VDP2_TVMD_HORZ_NORMAL_BE:
                width = 352;
                height = 480;
                clock_freq = CPU_CLOCK_SPEED_28MHZ;
                _state_vdp2()->regs.tvmd |= 0x0005;
                break;
        case VDP2_TVMD_HORZ_HIRESO_AE:
                width = 640;
                height = 480;
                clock_freq = CPU_CLOCK_SPEED_26MHZ;
                _state_vdp2()->regs.tvmd |= 0x0006;
                break;
        case VDP2_TVMD_HORZ_HIRESO_BE:
                width = 704;
                height = 480;
                clock_freq = CPU_CLOCK_SPEED_28MHZ;
                _state_vdp2()->regs.tvmd |= 0x0007;
                break;
        default:
                assert(false);
                return;
        }

        /* Update state */
        _state_vdp2()->tv.resolution.x = width;
        _state_vdp2()->tv.resolution.y = height;

        /* If clock frequency is not set, change to the correct
         * frequency */
        if (sys_clock != clock_freq) {
                bios_clock_speed_chg(clock_freq);
        }
}
