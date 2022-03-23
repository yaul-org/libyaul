/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>

#include <sys/dma-queue-internal.h>

#include <internal.h>

void
__dma_queue_init(dma_queue_t *queue)
{
        assert(queue != NULL);

        queue->xfer_table = __memalign(sizeof(dma_queue_t) * DMA_QUEUE_REQUESTS_MAX_COUNT, 256);
        queue->count = 0;

        assert(queue->xfer_table != NULL);
}

void
__dma_queue_deinit(dma_queue_t *queue)
{
        __free(queue->xfer_table);

        queue->count = 0;
}
