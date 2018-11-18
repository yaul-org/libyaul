/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>

#include <vdp1/env.h>
#include <vdp1/map.h>
#include <vdp1/cmdt.h>
#include <vdp1/vram.h>

#include "vdp-internal.h"

void *
vdp1_vram_texture_base_get(void)
{
        return (void *)_state_vdp1()->vram.texture_base;
}

struct vdp1_gouraud_table *
vdp1_vram_gouraud_base_get(void)
{
        return (struct vdp1_gouraud_table *)_state_vdp1()->vram.gouraud_base;
}

struct vdp1_clut *
vdp1_vram_clut_base_get(void)
{
        return (struct vdp1_clut *)_state_vdp1()->vram.clut_base;
}

void *
vdp1_vram_remaining_get(void)
{
        return (void *)_state_vdp1()->vram.remaining_base;
}
