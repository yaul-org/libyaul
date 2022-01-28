/*-
 * Copyright (c) 2004 Swedish Institute of Computer Science.
 * Copyright (c) 2012-2022 Israel Jacquez
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *    3. Neither the name of the Institute nor the names of its contributors
 *       may be used to endorse or promote products derived from this software
 *       without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "memb.h"

/*
 * Returns 1 if PTR is within bounds of the block pool NAME.
 */
#define MEMB_PTR_BOUND(name, ptr)                                              \
        (((int8_t *)(ptr) >= (int8_t *)(name)->pool) &&                        \
         ((int8_t *)(ptr) < ((int8_t *)(name)->pool +                          \
             ((name)->count * (name)->size))))

static inline uint32_t _block_index_wrap(const memb_t *memb, uint32_t index) __always_inline;

/*
 * Initialize a block pool MEMB.
 */
void
memb_init(memb_t *memb)
{
        for (uint32_t i = 0; i < memb->count; i++) {
                memb->ref[i].count = 0;
        }

        memb->next_index = 0;
        memb->alloc_count = 0;

        (void)memset(memb->pool, 0x00, memb->size * memb->count);
}

memb_t *
memb_memb_alloc(uint32_t block_count, uint32_t block_size, uint32_t align)
{
        block_count = max(block_count, 4UL);
        align = max(align, 4UL);

        const size_t ref_size = sizeof(memb_ref_t) * block_count;
        const size_t pool_size = block_size * block_count;

        /* Allocate all memory needed in a single request */
        void * const area = malloc(sizeof(memb_t) + ref_size + pool_size);

        if (area == NULL) {
                return NULL;
        }

        uintptr_t area_ptr;
        area_ptr = (uintptr_t)area;

        /* Have the memb_t structure be at the top of the allocation request.
         * This way, we can easily free */
        memb_t * const memb = (void *)area_ptr;
        area_ptr += sizeof(memb_t);

        memb->size = block_size;
        memb->count = block_count;

        memb->ref = (void *)area_ptr;
        area_ptr += ref_size;

        memb->pool = (void *)area_ptr;

        memb_init(memb);

        return memb;
}

void
memb_memb_free(memb_t *memb)
{
        if (memb == NULL) {
                return;
        }

        free(memb);
}

/*-
 * Allocate a unit block from the block pool MEMB.
 *
 * If successful, pointer to block is returned. Otherwise NULL is
 * returned for the following cases:
 *
 *   - MEMB is NULL; or
 *   - There are no free block(s) to allocate
 */
void *
memb_alloc(memb_t *memb)
{
        if (memb == NULL) {
                return NULL;
        }

        /* Are we full? */
        if (memb->alloc_count == memb->count) {
                return NULL;
        }

        while (true) {
                if (memb->ref[memb->next_index].count == 0) {
                        break;
                }

                memb->next_index = _block_index_wrap(memb, memb->next_index + 1);
        }

        void * const block = (void *)((uintptr_t)memb->pool +
            (memb->next_index * memb->size));

        memb->ref[memb->next_index].count = 1;
        memb->next_index = _block_index_wrap(memb, memb->next_index + 1);
        memb->alloc_count++;

        return block;
}

void *
memb_contiguous_alloc(memb_t *memb, uint32_t count)
{
        if (memb == NULL) {
                return NULL;
        }

        if (count == 0) {
                return NULL;
        }

        if (count == 1) {
                return memb_alloc(memb);
        }

        /* Are we full? */
        if (memb->alloc_count == memb->count) {
                return NULL;
        }

        uint32_t prev_index;
        prev_index = 0;
        uint32_t next_index;
        next_index = memb->next_index;

        for (uint32_t contiguous_count = 0; ; ) {
                if (memb->ref[next_index].count == 0) {
                        contiguous_count++;
                }

                if (contiguous_count == count) {
                        break;
                }

                prev_index = next_index;
                next_index = _block_index_wrap(memb, next_index + 1);

                /* Case for when the next index is wrapped to the beginning
                 * array */
                if ((contiguous_count > 0) && (next_index < prev_index)) {
                        return NULL;
                }
        }

        const uint32_t start_index = next_index - (count - 1);

        for (uint32_t i = start_index; i <= next_index; i++) {
                memb->ref[i].count = count;
        }

        memb->next_index = _block_index_wrap(memb, next_index + 1);
        memb->alloc_count += count;

        void * const block = (void *)((uintptr_t)memb->pool +
            (start_index * memb->size));

        return block;
}

/*
 * Free the unit block as dirty.
 *
 * If successful, 0 is returned. Otherwise -1 is returned if the address is not
 * within the bounds of the block pool MEMB.
 */
int
memb_free(memb_t *memb, void *addr)
{
        if (memb == NULL) {
                return -1;
        }

        /* Not within bounds */
        if (!MEMB_PTR_BOUND(memb, addr)) {
                return -1;
        }

        const uintptr_t pool_ptr = (uintptr_t)memb->pool;
        const uintptr_t addr_ptr = (uintptr_t)addr;

        const uint32_t block_index = (addr_ptr - pool_ptr) / memb->size;
        memb_ref_t * const ref = &memb->ref[block_index];

        const uint32_t contiguous_count = ref[0].count;

        for (uint32_t i = 0; i < contiguous_count; i++) {
                ref[i].count = 0;
        }

        memb->next_index = block_index;
        memb->alloc_count -= contiguous_count;

        return 0;
}

/*
 * Return the number of blocks allocated.
 *
 * If successful, the number of blocks allocated is returned. Otherwise -1 is
 * returned.
 */
int32_t
memb_size(memb_t *memb)
{
        if (memb == NULL) {
                return -1;
        }

        return memb->alloc_count;
}

/*
 * Determine if ADDR is within bounds of the block pool MEMB. Otherwise 0 is
 * returned.
 */
bool
memb_bounds(memb_t *memb, void *addr)
{
        return MEMB_PTR_BOUND(memb, addr);
}

static inline uint32_t __always_inline
_block_index_wrap(const memb_t *memb, uint32_t index)
{
        return ((index >= memb->count) ? 0 : index);
}
