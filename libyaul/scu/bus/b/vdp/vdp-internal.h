/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _VDP_INTERNAL_H_
#define _VDP_INTERNAL_H_

#include <math.h>

#include <scu-internal.h>

#include <vdp1/env.h>
#include <vdp1/map.h>
#include <vdp1/vram.h>

#include <vdp2/map.h>
#include <vdp2/scrn.h>
#include <vdp2/vram.h>

struct state_vdp1 {
        vdp1_registers_t *regs;
        vdp1_env_t const *current_env;
        vdp1_vram_partitions_t *vram_partitions;
} __aligned(16);

struct state_vdp2 {
        vdp2_registers_t *regs;
        vdp2_vram_ctl_t *vram_ctl;

        struct {
                vdp2_vram_t vram;
                const void *buffer;
                size_t len;
        } lncl;

        struct {
                vdp2_vram_t vram;
                const void *buffer;
                size_t len;
        } back;

        struct {
                scu_dma_handle_t *dma_handle;
                scu_dma_xfer_t *xfer_table;
        } commit;

        struct {
                int16_vec2_t resolution;
        } tv;
} __aligned(16);

static inline struct state_vdp1 * __always_inline
_state_vdp1(void)
{
        extern struct state_vdp1 __state_vdp1;

        return &__state_vdp1;
}

static inline struct state_vdp2 * __always_inline
_state_vdp2(void)
{
        extern struct state_vdp2 __state_vdp2;

        return &__state_vdp2;
}

extern void __vdp_init(void);

extern void __vdp2_commit(scu_dma_level_t level);
extern void __vdp2_commit_wait(scu_dma_level_t level);

#endif /* !_VDP_INTERNAL_H_ */
