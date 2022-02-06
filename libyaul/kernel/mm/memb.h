/*-
 * Copyright (c) 2004 Swedish Institute of Computer Science.
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

#ifndef _YAUL_KERNEL_MM_MEMB_H_
#define _YAUL_KERNEL_MM_MEMB_H_

#include <sys/cdefs.h>

#include <assert.h>
#include <stdint.h>
#include <stdbool.h>

__BEGIN_DECLS

/*
 * Statically declare a block pool.
 */
#define MEMB(name, structure, count, align)                                    \
static struct memb_ref                                                         \
    __CONCAT(name, _memb_ref)[((count) <= 0) ? 1 : (count)];                   \
                                                                               \
static __aligned(((align) <= 0) ? 4 : (align))                                 \
        structure __CONCAT1(name, _memb_mem)[((count) <= 0) ? 1 : (count)];    \
                                                                               \
static memb_t name = {                                                         \
        MEMB_TYPE_STATIC,                                                      \
        sizeof(structure),                                                     \
        ((count) <= 0) ? 1 : (count),                                          \
        &__CONCAT(name, _memb_ref)[0],                                         \
        0,                                                                     \
        0,                                                                     \
        (void *)&__CONCAT(name, _memb_mem)[0],                                 \
        NULL                                                                   \
}

typedef struct memb_ref {
        uint16_t count;
} __packed memb_ref_t;

typedef enum memb_type {
        MEMB_TYPE_STATIC,
        MEMB_TYPE_DYNAMIC,
        MEMB_TYPE_SET
} memb_type_t;

typedef struct memb {
        memb_type_t type;     /* Type of MEMB */
        uint32_t size;        /* Size (in bytes) of a unit block */
        uint32_t count;       /* Number of unit blocks in the block pool */
        memb_ref_t *refs;     /* Reference array */
        uint32_t next_index;  /* Index to next unreferenced block */
        uint32_t alloc_count; /* Number of allocated unit blocks */
        void *pool;

        void (*free)(void *p);
} memb_t;

void memb_init(memb_t *memb);
int memb_memb_init(memb_t *memb, void *pool, uint32_t block_count,
    uint32_t block_size);
int memb_memb_alloc(memb_t *memb, uint32_t block_count, uint32_t block_size,
    uint32_t align);
void memb_memb_free(memb_t *memb);

void *memb_alloc(memb_t *memb);
void *memb_contiguous_alloc(memb_t *memb, uint32_t count);
int memb_free(memb_t *memb, void *addr);

int32_t memb_size(memb_t *memb);
bool memb_bounds(memb_t *memb, void *addr);

__END_DECLS

#endif /* _YAUL_KERNEL_MM_MEMB_H_ */
