/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#ifndef _DMA_H_
#define _DMA_H_

enum dma_level {
        DMA_LEVEL_0,
        DMA_LEVEL_1,
        DMA_LEVEL_2
};

enum dma_mode {
        DMA_MODE_INDIRECT,
        DMA_MODE_DIRECT
};

struct dma_level_cfg {
        union {
                struct dma_mode_direct {
                        void *dst;
                        const void *src;
                        size_t len;
                } direct;

                struct dma_mode_indirect {
                        void *dst;
                        const void *src;
                        size_t len;
                        unsigned char end:31;
                } *indirect;
        } mode;

        unsigned short add;
        unsigned short factor;
};

extern bool     scu_dma_cpu_level_operating(enum dma_level);
extern bool     scu_dma_cpu_level_standby(enum dma_level);
extern void     scu_dma_cpu_init(void);
extern void     scu_dma_cpu_level_set(enum dma_level, enum dma_mode, struct dma_level_cfg *);
extern void     scu_dma_cpu_level_start(enum dma_level);
extern void     scu_dma_cpu_level_stop(enum dma_level);

#endif /* !_DMA_H_ */
