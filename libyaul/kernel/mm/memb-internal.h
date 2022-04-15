/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _KERNEL_MM_MEMB_INTERNAL_H_
#define _KERNEL_MM_MEMB_INTERNAL_H_

#include "memb.h"

#include <internal.h>

typedef struct memb_request {
        malloc_func_t malloc_func;
        memalign_func_t memalign_func;
        free_func_t free_func;

        uint32_t block_count;
        uint32_t block_size;
        uint32_t align;
} memb_request_t;

int __memb_memb_request_init(memb_t *memb, void *pool,
    const memb_request_t *request);
int __memb_memb_request_alloc(memb_t *memb, const memb_request_t *request);

int __memb_memb_init(memb_t *memb, void *pool, uint32_t block_count,
    uint32_t block_size);
int __memb_memb_alloc(memb_t *memb, uint32_t block_count, uint32_t block_size,
    uint32_t align);

#endif /* !_KERNEL_MM_MEMB_INTERNAL_H_ */
