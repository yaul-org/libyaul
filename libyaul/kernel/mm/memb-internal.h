/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_KERNEL_MM_MEMB_INTERNAL_H_
#define _YAUL_KERNEL_MM_MEMB_INTERNAL_H_

#include <sys/cdefs.h>

#include <internal.h>

#include "memb.h"

typedef struct memb_request {
        void *(*malloc)(size_t n);
        void *(*memalign)(size_t n, size_t align);
        void (*free)(void *p);

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

#endif /* !_YAUL_KERNEL_MM_MEMB_INTERNAL_H_ */
