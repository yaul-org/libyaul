/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>

#include <string.h>

#include <vdp1/env.h>
#include <vdp1/map.h>
#include <vdp1/cmdt.h>
#include <vdp1/vram.h>

#include "vdp-internal.h"

void
vdp1_vram_partitions_set(uint32_t cmdt_count, uint32_t texture_size,
    uint32_t gouraud_count, uint32_t clut_count)
{
#ifdef DEBUG
        /* We have to have at least one command */
        assert(cmdt_count > 0);
#endif /* DEBUG */

        /* One command table is allocated to us */
        uint32_t vram_size;
        vram_size = VDP1_VRAM_SIZE - sizeof(vdp1_cmdt_t);

        uint32_t cmdt_size;
        cmdt_size = cmdt_count * sizeof(vdp1_cmdt_t);

        uint32_t gouraud_size;
        gouraud_size = gouraud_count * sizeof(vdp1_gouraud_table_t);

        uint32_t clut_size;
        clut_size = clut_count * sizeof(vdp1_clut_t);

        uint32_t total_size;
        total_size = cmdt_size + texture_size + gouraud_size + clut_size;

        uint32_t remaining_size;
        remaining_size = VDP1_VRAM(VDP1_VRAM_SIZE) - total_size;

        assert(total_size <= vram_size);

        uint32_t vram_base;
        vram_base = VDP1_VRAM(sizeof(vdp1_cmdt_t));

        vdp1_vram_partitions_t *vram_partitions;
        vram_partitions = _state_vdp1()->vram_partitions;

        vram_partitions->cmdt_base = (vdp1_cmdt_t *)vram_base;
        vram_partitions->cmdt_size = cmdt_size;
        vram_base += cmdt_size;

        vram_partitions->texture_base = (void *)vram_base;
        vram_partitions->texture_size = texture_size;
        vram_base += texture_size;

        vram_partitions->gouraud_base = (vdp1_gouraud_table_t *)vram_base;
        vram_partitions->gouraud_size = gouraud_size;
        vram_base += gouraud_size;

        vram_partitions->clut_base = (vdp1_clut_t *)vram_base;
        vram_partitions->clut_size = clut_size;
        vram_base += clut_size;

        if (remaining_size == 0) {
                vram_partitions->remaining_base = NULL;
        } else {
                vram_partitions->remaining_base = (void *)vram_base;
        }

        vram_partitions->remaining_size = remaining_size;
}

void
vdp1_vram_partitions_get(vdp1_vram_partitions_t *vram_partitions)
{
        (void)memcpy(vram_partitions, _state_vdp1()->vram_partitions,
            sizeof(vdp1_vram_partitions_t));
}
