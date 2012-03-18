/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#ifndef _DMA_INTERNAL_H_
#define _DMA_INTERNAL_H_

/* Write and read directly to specified address. */
#define MEM_POKE(x, y)  (*(volatile uint32_t *)(x) = (y))
#define MEM_READ(x)     (*(volatile uint32_t *)(x))

/* Macros specific for processor. */
#define DMA(x)          (0x25fe0000 + (x))
#define DMA_LEVEL(x, y) (DMA(((x) << 5) + ((y) << 2)))

enum dma_regs {
        D0R,
        D0W,
        D0C,
        D0AD,
        D0EN,
        D0MD,
        D1R = 0,
        D1W,
        D1C,
        D1AD,
        D1EN,
        D1MD,
        D2R = 0,
        D2W,
        D2C,
        D2AD,
        D2EN,
        D2MD,
        DSTA = 0x7c
};

#endif /* !_DMA_INTERNAL_H_ */
