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

#include <vdp2/map.h>
#include <vdp2/scrn.h>
#include <vdp2/vram.h>

#define COMMIT_XFER_VDP2_REG_TVMD       0
#define COMMIT_XFER_VDP2_REGS           1
#define COMMIT_XFER_BACK_SCREEN_BUFFER  2
#define COMMIT_XFER_COUNT               3

struct state_vdp1 {
        struct {
                uint32_t cmdt_base;
                uint32_t texture_base;
                uint32_t gouraud_base;
                uint32_t clut_base;
                uint32_t remaining_base;
        } vram;

        vdp1_env_t const *current_env;

        vdp1_registers_t regs;
};

struct state_vdp2 {
        struct {
                int16_vector2_t resolution;
        } tv;

        struct {
                fix16_vector2_t scroll;
        } nbg0;

        struct {
                fix16_vector2_t scroll;
        } nbg1;

        struct {
                int16_vector2_t scroll;
        } nbg2;

        struct {
                int16_vector2_t scroll;
        } nbg3;

        struct {
                void *vram;
                void *buffer;
                uint32_t count;
        } back;

        struct {
                /* Align each SCU-DMA transfer table on a 64-byte boundary */
                scu_dma_xfer_t xfer_table[COMMIT_XFER_COUNT] __aligned(4 * 16);
                scu_dma_handle_t handle;
        } commit;

        vdp2_vram_ctl_t vram_ctl;

        vdp2_registers_t regs;
};

static inline struct state_vdp1 * __always_inline
_state_vdp1(void)
{
        extern struct state_vdp1 _internal_state_vdp1;

        return &_internal_state_vdp1;
}

static inline struct state_vdp2 * __always_inline
_state_vdp2(void)
{
        extern struct state_vdp2 _internal_state_vdp2;

        return &_internal_state_vdp2;
}

extern void _internal_vdp_init(void);

#endif /* !_VDP_INTERNAL_H_ */
