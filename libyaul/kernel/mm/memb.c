/*-
 * Copyright (c) 2004, Swedish Institute of Computer Science.
 * Copyright (c) 2012-2016, Israel Jacquez <mrkotfw@gmail.com>
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

#include "memb.h"

/*
 * Returns 1 if PTR is within bounds of the block pool NAME.
 */
#define MEMB_PTR_BOUND(name, ptr)                                              \
        (((int8_t *)(ptr) >= (int8_t *)(name)->m_bpool) &&                     \
         ((int8_t *)(ptr) < ((int8_t *)(name)->m_bpool +                       \
             ((name)->m_bnum * (name)->m_bsize))))

static void _incr_block_index(struct memb *);

/*
 * Initialize a block pool MB.
 */
void
memb_init(struct memb *mb)
{
        uint32_t bidx;

        for (bidx = 0; bidx < mb->m_bnum; bidx++) {
                mb->m_breftype[bidx] = MEMB_REF_AVAILABLE;
        }

        mb->m_size = 0;

        memset(mb->m_bpool, 0x00, mb->m_bsize * mb->m_bnum);
}

/*-
 * Allocate a unit block from the block pool MB.
 *
 * If successful, pointer to block is returned. Otherwise NULL is
 * returned for the following cases:
 *
 *   - MEMB MB is NULL; or
 *   - There are no free block(s) to allocate
 */
void *
memb_alloc(struct memb *mb)
{
        uint32_t bidx;
        uint32_t cur_bidx;
        int8_t *block;

        if (mb == NULL) {
                return NULL;
        }

        /* Are we full? */
        if (mb->m_bidx >= mb->m_bnum) {
                return NULL;
        }

        /* Try next-fit method */
        bidx = mb->m_bidx;

        if (mb->m_breftype[bidx] == MEMB_REF_AVAILABLE) {
                goto allocate;
        }

        /* Try first-fit method */
        cur_bidx = mb->m_bidx;

        do {
                _incr_block_index(mb);
                bidx = mb->m_bidx;

                if (mb->m_breftype[bidx] == MEMB_REF_AVAILABLE) {
                        goto allocate;
                }
        } while (cur_bidx != bidx);

        /* No free block was found, so we return NULL to indicate
         * failure to allocate block. */
        return NULL;

allocate:
        block = (int8_t *)mb->m_bpool + (bidx * mb->m_bsize);
        mb->m_breftype[bidx] = MEMB_REF_RESERVED;
        /* Increase to next block index */
        _incr_block_index(mb);

        mb->m_size++;

        return (void *)block;
}

/*
 * Free the unit block as dirty.
 *
 * If successful, 0 is returned. Otherwise -1 is returned if the
 * address is not within the bounds of the block pool MB.
 */
int
memb_free(struct memb *mb, void *addr)
{
        if (mb == NULL) {
                return -1;
        }

        /* Not within bounds. */
        if (!MEMB_PTR_BOUND(mb, addr)) {
                return -1;
        }

        int8_t *block;
        block = (int8_t *)mb->m_bpool;

        /* Walk through the list of blocks and try to find the block to
         * which the pointer address points to. */
        uint32_t bidx;

        for (bidx = 0; bidx < mb->m_bnum; bidx++) {
                if (block == (int8_t *)addr) {
                        if (mb->m_breftype[bidx] == MEMB_REF_AVAILABLE) {
                                break;
                        }

                        mb->m_breftype[bidx] = MEMB_REF_AVAILABLE;
                        mb->m_size--;

                        /* Set recently freed block to next block
                         * index */
                        mb->m_bidx = (mb->m_size == 0) ? 0 : bidx;

                        return 0;
                }

                block += mb->m_bsize;
        }

        return -1;
}

/*
 * Return the number of blocks allocated.
 *
 * If successful, the number of blocks allocated is returned. Otherwise
 * -1 is returned.
 */
int32_t
memb_size(struct memb *mb)
{
        if (mb == NULL) {
                return -1;
        }

        return mb->m_size;
}

/*
 * Determine if ADDR is within bounds of the block pool MB. Otherwise 0
 * is returned.
 *
 */
bool
memb_bounds(struct memb *mb, void *addr)
{
        return MEMB_PTR_BOUND(mb, addr);
}

static void __always_inline inline
_incr_block_index(struct memb *mb)
{
        if (mb->m_bidx >= (mb->m_bnum - 1)) {
                mb->m_bidx = 0;
        }

        mb->m_bidx++;
}
