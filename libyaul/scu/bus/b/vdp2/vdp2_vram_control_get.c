/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2/vram.h>

#include "vdp2-internal.h"

struct vram_ctl *
vdp2_vram_control_get(void)
{

        return &vram_ctl;
}
