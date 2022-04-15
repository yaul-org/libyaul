/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _KERNEL_SYS_DMA_QUEUE_INTERNAL_H_
#define _KERNEL_SYS_DMA_QUEUE_INTERNAL_H_

#include <sys/dma-queue.h>

#include <internal.h>

__BEGIN_DECLS

extern dma_queue_t *__dma_queue_alloc(void);
extern void __dma_queue_free(dma_queue_t *queue);
extern void __dma_queue_init(dma_queue_t *queue);
extern void __dma_queue_deinit(dma_queue_t *queue);

extern dma_queue_t *__dma_queue_request_alloc(malloc_func_t malloc_func);
extern void __dma_queue_request_free(dma_queue_t *queue, free_func_t free_func);
extern void __dma_queue_request_init(dma_queue_t *queue, memalign_func_t memalign_func);
extern void __dma_queue_request_deinit(dma_queue_t *queue, free_func_t free_func);

__END_DECLS

#endif /* !_KERNEL_SYS_DMA_QUEUE_INTERNAL_H_ */
