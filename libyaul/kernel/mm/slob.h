/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _SLOB_H_
#define _SLOB_H_

#include <stddef.h>
#include <stdint.h>

/*-
 * Restrictions
 *
 *   1. Heap size limit: Allocation requests bigger than
 *      SLOB_PAGE_BREAK_2ND cannot be serviced. This is due to the
 *      memory block manager not able to guarantee that sequential
 *      allocations of SLOB pages will be contiguous.
 */

/*
 * Adjust the number of pages to be statically allocated as needed. If
 * memory is quickly exhausted, increase the SLOB page count.
 */
#ifndef SLOB_PAGE_COUNT
#define SLOB_PAGE_COUNT 16
#endif /* !SLOB_PAGE_COUNT */

#define SLOB_PAGE_SIZE  0x1000
#define SLOB_PAGE_MASK  (~(SLOB_PAGE_SIZE - 1))

#define SLOB_PAGE_BREAK_1ST 0x0100
#define SLOB_PAGE_BREAK_2ND 0x0400

struct slob_stats {
        /* Number of free bytes (across all SLOB pages) */
        uint32_t ss_avail;

        /* Total hypothetical size available (all SLOB pages
         * allocated) */
        uint32_t ss_size;

        /* Number of allocated SLOB pages */
        uint32_t ss_usedpcnt;

        /* Total number of SLOB pages */
        uint32_t ss_pages;
};

void slob_init(void);
void *slob_alloc(size_t);
void *slob_realloc(void *, size_t);
void slob_free(void *);
int slob_stats(struct slob_stats *);

#endif /* _SLOB_H_ */
