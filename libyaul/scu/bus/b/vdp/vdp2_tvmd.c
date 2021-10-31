/*
 * Copyright (c) 2012-2019 Israel Jacquez
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
        _state_vdp2()->regs->tvmd &= 0x7EFF;
}

void
vdp2_tvmd_display_set(void)
{
        _state_vdp2()->regs->tvmd |= 0x8000;
}

void
vdp2_tvmd_display_res_get(uint16_t *width, uint16_t *height)
{
#ifdef DEBUG
        assert(width != NULL);
        assert(height != NULL);
#endif /* DEBUG */

        *width = _state_vdp2()->tv.resolution.x;
        *height = _state_vdp2()->tv.resolution.y;
}

void
vdp2_tvmd_display_res_set(vdp2_tvmd_interlace_t interlace, vdp2_tvmd_horz_t horizontal,
    vdp2_tvmd_vert_t vertical)
{
        const uint32_t sys_clock = bios_clock_speed_get();

        uint16_t width;
        uint16_t height;

        _state_vdp2()->regs->tvmd &= 0xFFCF;

        switch (vertical)  {
        case VDP2_TVMD_VERT_224:
                height = 224;
                break;
        case VDP2_TVMD_VERT_240:
                height = 240;
                _state_vdp2()->regs->tvmd |= 0x0010;
                break;
        case VDP2_TVMD_VERT_256:
                height = 256;
                _state_vdp2()->regs->tvmd |= 0x0020;
                break;
        default:
                height = 224;
                break;
        }

        _state_vdp2()->regs->tvmd &= 0xFF3F;

        switch (interlace)  {
        case VDP2_TVMD_INTERLACE_NONE:
                break;
        case VDP2_TVMD_INTERLACE_SINGLE:
                _state_vdp2()->regs->tvmd |= 0x0080;
                break;
        case VDP2_TVMD_INTERLACE_DOUBLE:
                height *= 2;
                _state_vdp2()->regs->tvmd |= 0x00C0;
                break;
        default:
                break;
        }

        uint16_t clock_speed;
        clock_speed = CPU_CLOCK_SPEED_26MHZ;

        _state_vdp2()->regs->tvmd &= 0xFFF0;

        switch (horizontal) {
        case VDP2_TVMD_HORZ_NORMAL_A:
                width = 320;
                break;
        case VDP2_TVMD_HORZ_NORMAL_B:
                width = 352;
                clock_speed = CPU_CLOCK_SPEED_28MHZ;
                _state_vdp2()->regs->tvmd |= 0x0001;
                break;
        case VDP2_TVMD_HORZ_HIRESO_A:
                width = 640;
                _state_vdp2()->regs->tvmd |= 0x0002;
                break;
        case VDP2_TVMD_HORZ_HIRESO_B:
                width = 704;
                clock_speed = CPU_CLOCK_SPEED_28MHZ;
                _state_vdp2()->regs->tvmd |= 0x0003;
                break;
        case VDP2_TVMD_HORZ_NORMAL_AE:
                width = 320;
                height = 480;
                _state_vdp2()->regs->tvmd |= 0x0004;
                break;
        case VDP2_TVMD_HORZ_NORMAL_BE:
                width = 352;
                height = 480;
                clock_speed = CPU_CLOCK_SPEED_28MHZ;
                _state_vdp2()->regs->tvmd |= 0x0005;
                break;
        case VDP2_TVMD_HORZ_HIRESO_AE:
                width = 640;
                height = 480;
                _state_vdp2()->regs->tvmd |= 0x0006;
                break;
        case VDP2_TVMD_HORZ_HIRESO_BE:
                width = 704;
                height = 480;
                clock_speed = CPU_CLOCK_SPEED_28MHZ;
                _state_vdp2()->regs->tvmd |= 0x0007;
                break;
        default:
                width = 320;
                break;
        }

        _state_vdp2()->tv.resolution.x = width;
        _state_vdp2()->tv.resolution.y = height;

        /* If clock frequency is not set, change to the correct
         * frequency */
        if (sys_clock != clock_speed) {
                bios_clock_speed_chg(clock_speed);
        }
}

void
vdp2_tvmd_vblank_in_next_wait(uint32_t count)
{
        for (uint32_t i = 0; i < count; i++) {
                if (vdp2_tvmd_display()) {
                        vdp2_tvmd_vblank_out_wait();
                }

                vdp2_tvmd_vblank_in_wait();
        }
}

void
vdp2_tvmd_border_set(bool enable)
{
        /* If BDCLMD (0x0100 is set and DISP has never been set, the back screen
         * will not display properly */
        _state_vdp2()->regs->tvmd &= 0xFEFF;
        _state_vdp2()->regs->tvmd |= 0x8000;

        if (enable) {
                _state_vdp2()->regs->tvmd |= 0x0100;
        }
}
