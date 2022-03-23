/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_KERNEL_SYS_DMA_QUEUE_H_
#define _YAUL_KERNEL_SYS_DMA_QUEUE_H_

#include <stdint.h>

#include <scu/dma.h>

__BEGIN_DECLS

#define DMA_QUEUE_REQUESTS_MAX_COUNT (16)
#define DMA_QUEUE_REQUESTS_MASK      (DMA_QUEUE_REQUESTS_MAX_COUNT - 1)

typedef struct dma_queue {
        scu_dma_xfer_t *xfer_table;
        uint32_t count;
} __aligned(4) dma_queue_t;

static_assert(sizeof(struct dma_queue) == 8);

extern dma_queue_t *dma_queue_alloc(void);
extern void dma_queue_free(dma_queue_t *queue);
extern void dma_queue_init(dma_queue_t *queue);
extern void dma_queue_deinit(dma_queue_t *queue);
extern int32_t dma_queue_enqueue(dma_queue_t *queue, void *dst,
    const void *src, size_t len);
extern void dma_queue_clear(dma_queue_t *queue);
extern uint32_t dma_queue_count_get(const dma_queue_t *queue);

__END_DECLS

#endif /* !_YAUL_KERNEL_SYS_DMA_QUEUE_H_ */
