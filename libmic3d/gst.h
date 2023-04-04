/*
 * Copyright (c) 2022-2023 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _MIC3D_GST_H_
#define _MIC3D_GST_H_

#include <gamemath/fix16.h>

#include <cpu/dmac.h>
#include <scu/dma.h>

#include <mic3d/types.h>

#include "state.h"

typedef struct gst {
    vdp1_vram_t vram_base;
    gst_slot_t slot_base;

    scu_dma_level_t dma_level;
    cpu_dmac_channel_t dma_channel;
} gst_t;

static inline gst_slot_t __always_inline
__gst_slot_calculate(gst_slot_t gst_slot)
{
    gst_t * const gst = __state.gst;

    return (gst->slot_base + gst_slot);
}

void __gst_init(void);

void __gst_put(const vdp1_gouraud_table_t *gouraud_tables, vdp1_vram_t vram_base, uint32_t put_count);

#endif /* _MIC3D_GST_H_ */
