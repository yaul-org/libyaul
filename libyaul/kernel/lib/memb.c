/*-
 * Copyright (c) 2004, Swedish Institute of Computer Science.
 * Copyright (c) 2012, Israel Jacquez <mrkotfw@gmail.com>
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

static void memb_block_next(struct memb *);

/*
 * Initialize a block pool MB.
 */
void
memb_init(struct memb *mb)
{
        uint32_t bidx;

        for (bidx = 0; bidx < mb->m_bnum; bidx++)
                mb->m_breftype[bidx] = MEMB_REF_AVAILABLE;

        memset(mb->m_bpool, 0, mb->m_bsize * mb->m_bnum);
}

/*-
 * Allocate a unit block from the block pool MB.
 *
 * If successful, 0 is returned. Otherwise a -1 is returned for the
 * following cases:
 *
 *   - A pointer to the lock if successful or
 *   - A NULL pointer
 */
void *
memb_alloc(struct memb *mb)
{
        uint32_t bidx;
        uint32_t bcuridx;
        int8_t *block;

        if (mb == NULL)
                return NULL;

        /* Try next-fit method */
        bidx = mb->m_bidx;
        if (mb->m_breftype[bidx] == MEMB_REF_AVAILABLE)
                goto allocate;

        /* Try first-fit method */
        bcuridx = mb->m_bidx;
        do {
                memb_block_next(mb);
                bidx = mb->m_bidx;
                if (mb->m_breftype[bidx] == MEMB_REF_AVAILABLE)
                        goto allocate;
        } while (bcuridx != bidx);

        /* No free block was found, so we return NULL to indicate
         * failure to allocate block. */
        return NULL;

allocate:
        block = (int8_t *)mb->m_bpool + (bidx * mb->m_bsize);
        mb->m_breftype[bidx] = MEMB_REF_RESERVED;
        /* Increase to next block index */
        memb_block_next(mb);

        return (void *)block;
}

/*
 * Free the unit block as dirty.
 *
 * If successful, 0 is returned. Otherwise a -1 is returned if the
 * address is not within the bounds of the block pool MB.
 */
int
memb_free(struct memb *mb, void *addr)
{
        uint32_t bidx;
        int8_t *block;

        /* Not within bounds. */
        if (!MEMB_PTR_BOUND(mb, addr))
                return -1;

        block = (int8_t *)mb->m_bpool;

        /* Walk through the list of blocks and try to find the block to
         * which the pointer address points to. */
        for (bidx = 0; bidx < mb->m_bnum; bidx++) {
                if (block == (int8_t *)addr) {
                        if (mb->m_breftype[bidx] == MEMB_REF_AVAILABLE)
                                break;
                        mb->m_breftype[bidx] = MEMB_REF_AVAILABLE;
                        /* Set recently freed block to next block
                         * index */
                        mb->m_bidx = bidx;
                        return 0;
                }
                block += mb->m_bsize;
        }

        return -1;
}

static void
memb_block_next(struct memb *mb)
{
        if (mb->m_bidx >= (mb->m_bnum - 1))
                mb->m_bidx = 0;
        mb->m_bidx++;
}
