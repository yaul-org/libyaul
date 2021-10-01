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

#define DMA_QUEUE_TAG_IMMEDIATE         (0)
#define DMA_QUEUE_TAG_VBLANK_IN         (1)
#define DMA_QUEUE_TAG_VBLANK_OUT        (2)
#define DMA_QUEUE_TAG_INVALID           (255)
#define DMA_QUEUE_TAG_COUNT             (3)

typedef struct dma_queue_transfer {
/* DMA request unknown */
#define DMA_QUEUE_STATUS_UNKNOWN        (0x00)
/* DMA request not yet processed */
#define DMA_QUEUE_STATUS_UNPROCESSED    (0x01)
/* DMA request is being processed */
#define DMA_QUEUE_STATUS_PROCESSING     (0x02)
/* DMA request explicitly canceled */
#define DMA_QUEUE_STATUS_CANCELED       (0x04)
/* DMA request has been completed */
#define DMA_QUEUE_STATUS_COMPLETE       (0x08)
        uint8_t status;
        void *work;
} __aligned(4) dma_queue_transfer_t;

typedef void (*dma_queue_request_hdl_t)(const dma_queue_transfer_t *);

extern int8_t dma_queue_enqueue(const scu_dma_handle_t *, uint8_t,
    dma_queue_request_hdl_t, void *);
extern int8_t dma_queue_simple_enqueue(uint8_t, void *, void *, size_t);
extern void dma_queue_tag_clear(uint8_t);
extern void dma_queue_clear(void);
extern uint32_t dma_queue_flush(uint8_t);
extern void dma_queue_flush_wait(void);
extern uint32_t dma_queue_count_get(uint8_t);
extern uint32_t dma_queue_capacity_get(void);

__END_DECLS

#endif /* !_YAUL_KERNEL_SYS_DMA_QUEUE_H_ */
