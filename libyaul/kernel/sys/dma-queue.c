/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include <cpu/cache.h>
#include <cpu/intc.h>
#include <cpu/registers.h>

#include <sys/dma-queue.h>

#include <scu-internal.h>

#include <internal.h>

dma_queue_t *
dma_queue_alloc(void)
{
        dma_queue_t * const queue = malloc(sizeof(dma_queue_t));

        dma_queue_init(queue);

        return queue;
}

void
dma_queue_free(dma_queue_t *queue)
{
        assert(queue != NULL);

        dma_queue_deinit(queue);

        free(queue);
}

void
dma_queue_init(dma_queue_t *queue)
{
        assert(queue != NULL);

        queue->xfer_table = memalign(sizeof(dma_queue_t) * DMA_QUEUE_REQUESTS_MAX_COUNT, 256);
        queue->count = 0;

        assert(queue->xfer_table != NULL);
}

void
dma_queue_deinit(dma_queue_t *queue)
{
        free(queue->xfer_table);
        queue->count = 0;
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
