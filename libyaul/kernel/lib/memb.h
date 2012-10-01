/*-
 * Copyright (c) 2004, Swedish Institute of Computer Science.
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

#ifndef _MEMB_H_
#define _MEMB_H_

#include <inttypes.h>

#include "common/common.h"

/*
 * Statically declare a block pool.
 */
#define MEMB(name, structure, num, align)                                      \
static enum memb_ref_type CC_CONCAT(name, _memb_refcnt)[(num)]                 \
        __attribute__ ((unused));                                              \
static __attribute__ ((aligned((align) == 0 ? 2 : (align))))                   \
        structure CC_CONCAT(name, _memb_mem)[(num)] __attribute__ ((unused));  \
static struct memb name __attribute__ ((unused)) = {                           \
        sizeof(structure),                                                     \
        num,                                                                   \
        &CC_CONCAT(name, _memb_refcnt)[0],                                     \
        0,                                                                     \
        (void *)&CC_CONCAT(name, _memb_mem)[0]                                 \
}

/*
 * Returns 1 if PTR is within bounds of the block pool NAME.
 */
#define MEMB_PTR_BOUND(name, ptr)                                              \
        (((int8_t *)(ptr) >= (int8_t *)(name)->m_bpool) &&                     \
                ((int8_t *)(ptr) < ((int8_t *)(name)->m_bpool +                \
                        ((name)->m_bnum * (name)->m_bsize))))

enum memb_ref_type {
        MEMB_REF_AVAILABLE,
        MEMB_REF_RESERVED
};

struct memb {
        uint32_t m_bsize; /* Size (in bytes) of a unit block */
        uint32_t m_bnum; /* Number of unit blocks in the block pool */
        enum memb_ref_type *m_breftype; /* Reference type array */
        uint32_t m_bidx; /* Index to next unreferenced block */
        void *m_bpool;
};

void memb_init(struct memb *);
void *memb_alloc(struct memb *);
int memb_free(struct memb *, void *);

#endif /* _MEMB_H_ */
