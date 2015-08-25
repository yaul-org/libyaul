/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include "globals.h"

struct cons cons;
struct smpc_peripheral_digital digital_pad;
uint32_t tick = 0;
char *text = NULL;
size_t text_len = 0;

void
sleep(fix16_t seconds)
{
        uint16_t frames;
        frames = fix16_to_int(fix16_mul(fix16_from_int(60), seconds));

        uint16_t t;
        for (t = 0; t < frames; t++) {
                vdp2_tvmd_vblank_out_wait();
                vdp2_tvmd_vblank_in_wait();
        }
}
