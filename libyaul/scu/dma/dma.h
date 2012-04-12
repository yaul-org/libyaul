/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#ifndef _SCU_DMA_H_
#define _SCU_DMA_H_

#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <sys/cdefs.h>

#include <common.h>

enum dma_level {
        DMA_LEVEL_0,
        DMA_LEVEL_1,
        DMA_LEVEL_2
};

enum dma_mode {
        DMA_MODE_DIRECT,
        DMA_MODE_INDIRECT
};

enum dma_strt_factor_type {
        DMA_STRT_FACTOR_VBLANK_IN,
        DMA_STRT_FACTOR_VBLANK_OUT,
        DMA_STRT_FACTOR_HBLANK_IN,
        DMA_STRT_FACTOR_TIMER_0,
        DMA_STRT_FACTOR_TIMER_1,
        DMA_STRT_FACTOR_SOUND_REQ,
        DMA_STRT_FACTOR_SPRITE_DRAW_END,
        DMA_STRT_FACTOR_ENABLE
};

#define DMA_MODE_INDIRECT_END 0x8000000

struct dma_level_cfg {
        union {
                struct dma_mode_direct {
                        void *dst;
                        const void *src;
                        size_t len;
                } direct;

                struct dma_mode_indirect {
                        struct dma_mode_indirect_table {
                                size_t len;
                                void *dst;
                                const void *src;
                        } __attribute ((__packed__)) *tbl;
                        uint32_t nelems;
                } indirect;
        } mode;

        uint32_t add;
        enum dma_strt_factor_type starting_factor;
};

extern bool     scu_dma_cpu_level_operating(enum dma_level);
extern bool     scu_dma_cpu_level_standby(enum dma_level);
extern void     scu_dma_cpu_init(void);
extern void     scu_dma_cpu_level_set(enum dma_level, enum dma_mode, struct dma_level_cfg *);
extern void     scu_dma_cpu_level_start(enum dma_level);
extern void     scu_dma_cpu_level_stop(enum dma_level);

#endif /* !_SCU_DMA_H_ */
