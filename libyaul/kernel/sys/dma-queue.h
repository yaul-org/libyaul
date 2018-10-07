/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _DMA_QUEUE_H_
#define _DMA_QUEUE_H_

#include <stdint.h>

#include <scu/dma.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define DMA_QUEUE_TAG_IMMEDIATE         0
#define DMA_QUEUE_TAG_VBLANK_IN         1
#define DMA_QUEUE_TAG_VBLANK_OUT        2
#define DMA_QUEUE_TAG_COUNT             3

extern void dma_queue_init(void);
extern int8_t dma_queue_enqueue(const void *, uint8_t, void (*)(void *), void *);
extern void dma_queue_clear(void);
extern int8_t dma_queue_flush(uint8_t);
extern void dma_queue_flush_wait(uint8_t);
extern uint32_t dma_queue_count_get(uint8_t);
extern uint32_t dma_queue_capacity_get(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !_DMA_QUEUE_H_ */
