/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>
#include <stdlib.h>

#include <cpu/cache.h>
#include <cpu/intc.h>

#include "dma-queue-internal.h"

dma_queue_t *
dma_queue_alloc(void)
{
        return __dma_queue_request_alloc(malloc);
}

void
dma_queue_free(dma_queue_t *queue)
{
        __dma_queue_request_free(queue, free);
}

void
dma_queue_init(dma_queue_t *queue)
{
        __dma_queue_request_init(queue, memalign);
}

void
dma_queue_deinit(dma_queue_t *queue)
{
        __dma_queue_request_deinit(queue, free);
}

int32_t
dma_queue_enqueue(dma_queue_t *queue, void *dst, const void *src, size_t len)
{
        assert(queue != NULL);

        int32_t status;
        status = 0;

        const uint32_t sr_mask = cpu_intc_mask_get();
        cpu_intc_mask_set(15);

        if (queue->count == DMA_QUEUE_REQUESTS_MAX_COUNT) {
                status = -1;

                goto exit;
        }

        scu_dma_xfer_t * const xfer = &queue->xfer_table[queue->count];

        xfer->dst = (uint32_t)dst;
        xfer->src = (uint32_t)src | SCU_DMA_INDIRECT_TABLE_END;
        xfer->len = len;

        if (queue->count > 0) {
                scu_dma_xfer_t * const prev_xfer = &queue->xfer_table[queue->count - 1];

                prev_xfer->src &= ~SCU_DMA_INDIRECT_TABLE_END;
        }

        queue->count++;

exit:
        cpu_intc_mask_set(sr_mask);

        return status;
}

void
dma_queue_clear(dma_queue_t *queue)
{
        assert(queue != NULL);

        const uint32_t sr_mask = cpu_intc_mask_get();
        cpu_intc_mask_set(15);

        queue->count = 0;

        cpu_intc_mask_set(sr_mask);
}

uint32_t
dma_queue_count_get(const dma_queue_t *queue)
{
        return queue->count;
}
