/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_KERNEL_SYS_DMA_QUEUE_INTERNAL_H_
#define _YAUL_KERNEL_SYS_DMA_QUEUE_INTERNAL_H_

#include <sys/dma-queue.h>

__BEGIN_DECLS

extern void __dma_queue_init(dma_queue_t *queue);
extern void __dma_queue_deinit(dma_queue_t *queue);

__END_DECLS

#endif /* !_YAUL_KERNEL_SYS_DMA_QUEUE_INTERNAL_H_ */
