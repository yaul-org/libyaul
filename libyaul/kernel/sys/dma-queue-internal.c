/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include "dma-queue-internal.h"

dma_queue_t *
__dma_queue_alloc(void)
{
        return __dma_queue_request_alloc(__malloc);
}

void
__dma_queue_free(dma_queue_t *queue)
{
        __dma_queue_request_free(queue, __free);
}

void
__dma_queue_init(dma_queue_t *queue)
{
        __dma_queue_request_init(queue, __memalign);
}

void
__dma_queue_deinit(dma_queue_t *queue)
{
        __dma_queue_request_deinit(queue, __free);
}

dma_queue_t *
__dma_queue_request_alloc(malloc_func_t malloc_func)
{
        dma_queue_t * const queue = malloc_func(sizeof(dma_queue_t));

        dma_queue_init(queue);

        return queue;
}

void
__dma_queue_request_free(dma_queue_t *queue, free_func_t free_func)
{
        assert(queue != NULL);

        dma_queue_deinit(queue);

        free_func(queue);
}

void
__dma_queue_request_init(dma_queue_t *queue, memalign_func_t memalign_func)
{
        assert(queue != NULL);

        queue->xfer_table = memalign_func(sizeof(dma_queue_t) * DMA_QUEUE_REQUESTS_MAX_COUNT, 256);
        queue->count = 0;

        assert(queue->xfer_table != NULL);
}

void
__dma_queue_request_deinit(dma_queue_t *queue, free_func_t free_func)
{
        free_func(queue->xfer_table);
        queue->count = 0;
}
