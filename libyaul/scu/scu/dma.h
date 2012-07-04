/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _SCU_DMA_H_
#define _SCU_DMA_H_

#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <sys/cdefs.h>

#include <common.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

enum dma_level {
        DMA_LEVEL_0,
        DMA_LEVEL_1,
        DMA_LEVEL_2
};

enum dma_mode {
        DMA_MODE_DIRECT,
        DMA_MODE_INDIRECT
};

enum dma_mode_strt_factor_type {
        DMA_MODE_START_FACTOR_VBLANK_IN,
        DMA_MODE_START_FACTOR_VBLANK_OUT,
        DMA_MODE_START_FACTOR_HBLANK_IN,
        DMA_MODE_START_FACTOR_TIMER_0,
        DMA_MODE_START_FACTOR_TIMER_1,
        DMA_MODE_START_FACTOR_SOUND_REQ,
        DMA_MODE_START_FACTOR_SPRITE_DRAW_END,
        DMA_MODE_START_FACTOR_ENABLE
};

enum dma_mode_update_type {
        DMA_MODE_UPDATE_RUP = 0x00010000,
        DMA_MODE_UPDATE_WUP = 0x00000100
};

#define DMA_MODE_INDIRECT_END 0x8000000

struct dma_level_cfg {
        union {
                struct {
                        void *dst;
                        const void *src;
                        size_t len;
                } direct;

                struct {
                        struct {
                                size_t len;
                                void *dst;
                                const void *src;
                        } __attribute ((__packed__)) *tbl;
                        uint32_t nelems;
                } indirect;
        } mode;

        uint32_t add;
        enum dma_mode_strt_factor_type starting_factor;
        enum dma_mode_update_type update;
};

extern void scu_dma_cpu_init(void);
extern void scu_dma_cpu_level_set(enum dma_level, enum dma_mode, struct dma_level_cfg *);
extern void scu_dma_cpu_level_start(enum dma_level);
extern void scu_dma_cpu_level_stop(enum dma_level);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* !_SCU_DMA_H_ */
