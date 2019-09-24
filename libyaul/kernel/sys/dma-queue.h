/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _DMA_QUEUE_H_
#define _DMA_QUEUE_H_

#include <stdint.h>

#include <scu/dma.h>

__BEGIN_DECLS

#define DMA_QUEUE_TAG_IMMEDIATE         (0)
#define DMA_QUEUE_TAG_VBLANK_IN         (1)
#define DMA_QUEUE_TAG_VBLANK_OUT        (2)
#define DMA_QUEUE_TAG_INVALID           (255)
#define DMA_QUEUE_TAG_COUNT             (3)

struct dma_queue_transfer {
/* DMA request has been completed */
#define DMA_QUEUE_STATUS_COMPLETE       (0)
/* DMA request not yet processed */
#define DMA_QUEUE_STATUS_INCOMPLETE     (1)
/* DMA request explicitly canceled */
#define DMA_QUEUE_STATUS_CANCELED       (2)
/* DMA request unknown */
#define DMA_QUEUE_STATUS_UNKNOWN        (255)
        uint8_t status;
        void *work;
} __aligned(4);

extern void dma_queue_init(void);
extern int8_t dma_queue_enqueue(const struct scu_dma_reg_buffer *, uint8_t, void (*)(const struct dma_queue_transfer *), void *);
extern void dma_queue_tag_clear(uint8_t);
extern void dma_queue_clear(void);
extern uint32_t dma_queue_flush(uint8_t);
extern void dma_queue_flush_wait(void);
extern uint32_t dma_queue_count_get(uint8_t);
extern uint32_t dma_queue_capacity_get(void);

__END_DECLS

#endif /* !_DMA_QUEUE_H_ */
