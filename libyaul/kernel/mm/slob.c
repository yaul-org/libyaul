/*
 * Copyright (c) 2012-2018 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 * Theo Berkau <cwx@cyberwarriorx.com>
 */

#include <sys/cdefs.h>
#include <sys/queue.h>

#include <mm/memb.h>

#include <assert.h>
#include <string.h>
#include <stdbool.h>

#include "slob.h"

#define SLOB_BLOCK_UNIT         (sizeof(struct slob_block))
#define SLOB_BLOCK_UNITS(s)     (((s) + SLOB_BLOCK_UNIT - 1) / SLOB_BLOCK_UNIT)
#define SLOB_BLOCK_SIZE(s)      ((s) * SLOB_BLOCK_UNIT)

#define SLOB_PAGE(sb)           ((struct slob_page *)((uintptr_t)(sb) & SLOB_PAGE_MASK))

#define ROUND(x)                ((((x) & 1) == 0) ? x : ((x) + 2 - ((x) & 1)))

#define SLOB_PAGE_ACTUAL_SIZE   (SLOB_PAGE_SIZE -                              \
    sizeof(struct slob_block) -                                                \
    sizeof(struct slob_block) -                                                \
    sizeof(struct slob_page_meta) -                                            \
    sizeof(SLIST_ENTRY(slob_page)))

#define SLOB_PAGE_LIST_END      (0xFFFFFFFF)

struct slob_page;

struct slob_page_meta {
        uint32_t spm_bunits; /* Free number of block units */
        struct slob_block *spm_bfree; /* Pointer to first free block */

        unsigned int : 32;
        unsigned int : 32;
        unsigned int : 32;
} __packed;

struct slob_block {
        /* The start of the address of the request returned to the
         * requestor must be on a 4 byte boundary due to alignment
         * issues */
        uint32_t sb_bunits;
} __packed;

struct slob_page {
        /* Page must be the first member in order to be aligned on a
         * SLOB page size boundry. */
        struct slob_block sp_page[SLOB_BLOCK_UNITS(SLOB_PAGE_ACTUAL_SIZE) + 2];

        /* Page meta information */
        struct slob_page_meta sp_meta;

        SLIST_ENTRY(slob_page) sp_list; /* Link to next page */
} __packed;

SLIST_HEAD(slob_page_list, slob_page);

static struct slob_block *slob_block_alloc(struct slob_page *, uint32_t);
static uint32_t slob_block_units(struct slob_block *);
static uint32_t slob_block_units_ptr(void *);

static void slob_block_list_set(struct slob_block *, struct slob_block *, uint32_t);
static struct slob_block *slob_block_list_next(struct slob_block *);
static bool slob_block_list_last(struct slob_block *);

static struct slob_page_list *slob_page_list_select(uint32_t);

static int slob_page_alloc(struct slob_page_list *);
static int slob_page_free(struct slob_page_list *, struct slob_page *);
static uint32_t slob_page_list_block_units(struct slob_page_list *);
static uint32_t slob_page_list_count(struct slob_page_list *);

static struct slob_page_list
slob_small_page_list = SLIST_HEAD_INITIALIZER(slob_small_page_list);
static struct slob_page_list
slob_medium_page_list = SLIST_HEAD_INITIALIZER(slob_medium_page_list);
static struct slob_page_list
slob_large_page_list = SLIST_HEAD_INITIALIZER(slob_large_page_list);

MEMB(slob_pages, struct slob_page, SLOB_PAGE_COUNT, SLOB_PAGE_SIZE);

/*
 * Initialize the SLOB allocator.
 */
void
slob_init(void)
{
        memb_init(&slob_pages);
}

/*
 * Allocate arbritrary amount of memory.
 */
void *
slob_alloc(size_t size)
{
        struct slob_page_list *spl;
        struct slob_page *sp;

        struct slob_block *block;

        uint32_t bunits;
        uint32_t btotal;

        if (size == 0) {
                return NULL;
        }

        /* 1+ unit for the prepended SLOB header and rounded to the
         * nearest multiple of 2 */
        bunits = ROUND(SLOB_BLOCK_UNITS(size) + 1);
        btotal = SLOB_BLOCK_UNITS(SLOB_PAGE_ACTUAL_SIZE);

        /* See restrictions */
        if (bunits > btotal) {
                return NULL;
        }

        /* Choose the right list of SLOB pages */
        spl = slob_page_list_select(bunits);

        block = NULL;
        SLIST_FOREACH(sp, spl, sp_list) {
                /* If the request is too large, there is no need to walk
                 * the free block list */
                if (bunits > sp->sp_meta.spm_bunits) {
                        continue;
                }

                if ((block = slob_block_alloc(sp, bunits)) != NULL) {
                        break;
                }
        }

        if (block == NULL) {
                /* There is no SLOB page that can satisfy the request
                 * allocation */
                if ((slob_page_alloc(spl)) < 0) {
                        /* Completely exhausted pool of SLOB pages */
                        return NULL;
                }

                /* Okay, let's try again */
                sp = SLIST_FIRST(spl);
                block = slob_block_alloc(sp, bunits);
        }

        return block + 1;
}

/*
 * Change the size of the pre-allocated memory.
 */
void *
slob_realloc(void *old __unused, size_t nsize __unused)
{
        return NULL;
}

/*
 * Free memory associated with the pointer ADDR.
 */
void
slob_free(void *addr)
{
        struct slob_page_list *spl;
        struct slob_page *sp;

        struct slob_block *sb;
        struct slob_block *bbase;
        struct slob_block *block;
        struct slob_block *bnext;
        struct slob_block *blast;

        uint32_t bunits;

        if (addr == NULL) {
                return;
        }

        assert(addr != NULL);

        /* Check if ADDR is within bounds of a SLOB managed memory */
        assert((memb_bounds(&slob_pages, addr)));

        sb = (struct slob_block *)addr - 1;
        sp = SLOB_PAGE(sb);

        bunits = slob_block_units(sb);

        /* Case for the just-freed block has emptied the SLOB page of
         * any allocations */
        if ((sp->sp_meta.spm_bunits + bunits) == SLOB_BLOCK_UNITS(SLOB_PAGE_ACTUAL_SIZE)) {
                /* Get SLOB page list head */
                spl = slob_page_list_select(bunits);

                sp->sp_meta.spm_bunits += bunits;

                int error;
                error = slob_page_free(spl, sp);
                assert(error == 0);

                return;
        }

        /* Case for when a SLOB page has no free block units */
        if (sp->sp_meta.spm_bunits == 0) {
                sp->sp_meta.spm_bfree = sb;

                bbase = &sp->sp_page[0];
                blast = bbase + SLOB_BLOCK_UNITS(SLOB_PAGE_ACTUAL_SIZE);
                slob_block_list_set(sb, blast, bunits);

                goto free;
        }

        block = sp->sp_meta.spm_bfree;
        bnext = slob_block_list_next(block);

        if (sb < block) {
                if ((sb + bunits) == block) {
                        slob_block_list_set(sb, bnext, slob_block_units(block) + bunits);
                } else {
                        slob_block_list_set(sb, block, bunits);
                }

                sp->sp_meta.spm_bfree = sb;

                goto free;
        }

        if (sb == block) {
                return;
        }

        /* Walk the free-list until the block adjacent
         * to the just-freed block is found */
        while (sb > bnext) {
                block = bnext;
                bnext = slob_block_list_next(block);
        }

        if ((block + slob_block_units(block)) == sb) {
                /* Coalesce free block BLOCK with SB */

                /* Coalesce adjacent block from the left */
                slob_block_list_set(block, bnext, slob_block_units(block) + bunits);
        } else {
                /* Link BLOCK to SB and SB to BNEXT */
                slob_block_list_set(block, sb, slob_block_units(block));
                slob_block_list_set(sb, bnext, bunits);
        }

        if (((sb + bunits) == bnext) && !(slob_block_list_last(bnext))) {
                /* Coalesce adjacent block from the right */
                slob_block_list_set(block,
                    slob_block_list_next(bnext),
                    slob_block_units(block) + slob_block_units(bnext));
        }

free:
        sp->sp_meta.spm_bunits += bunits;
}

/*-
 * Return SLOB statistics.
 *
 * If successful, 0 is returned. Otherwise a -1 is returned for the
 * following cases:
 *
 *   - STATS is NULL
 */
int
slob_stats(struct slob_stats *stats)
{
        if (stats == NULL) {
                return -1;
        }

        /* Count the number of used SLOB pages */
        uint32_t usedpcnt;
        usedpcnt = 0;

        usedpcnt += slob_page_list_count(&slob_small_page_list);
        usedpcnt += slob_page_list_count(&slob_medium_page_list);
        usedpcnt += slob_page_list_count(&slob_large_page_list);

        uint32_t freepcnt;
        freepcnt = SLOB_PAGE_COUNT - usedpcnt;

        uint32_t bavail;
        bavail = freepcnt * SLOB_BLOCK_UNITS(SLOB_PAGE_ACTUAL_SIZE);
        bavail += slob_page_list_block_units(&slob_small_page_list);
        bavail += slob_page_list_block_units(&slob_medium_page_list);
        bavail += slob_page_list_block_units(&slob_large_page_list);

        stats->ss_usedpcnt = usedpcnt;
        stats->ss_pages = SLOB_PAGE_COUNT;

        stats->ss_avail = (bavail / 2) * SLOB_BLOCK_UNIT;
        /* For every allocation request, one block unit is required for bookkeeping*/
        stats->ss_size = (SLOB_PAGE_ACTUAL_SIZE / 2) * SLOB_PAGE_COUNT;

        return 0;
}

/*
 * Link two SLOB blocks together. Namely, have SB link to SB_NEXT.
 */
static void
slob_block_list_set(struct slob_block *sb, struct slob_block *sb_next,
    uint32_t bunits)
{
        struct slob_page *sp;
        uint32_t bofs;

        sp = SLOB_PAGE(sb);
        bofs = sb_next - sp->sp_page;

        sb[0].sb_bunits = bunits;
        sb[1].sb_bunits = bofs;
}

/*
 * Terminate list.
 */
static void
slob_block_list_term(struct slob_block *sb, uint32_t bunits)
{
        sb[0].sb_bunits = bunits;
        sb[1].sb_bunits = SLOB_PAGE_LIST_END;
}

/*
 * Return a pointer to the next free block in the free block linked
 * list.
 */
static struct slob_block *
slob_block_list_next(struct slob_block *sb)
{
        struct slob_page *sp;
        sp = SLOB_PAGE(sb);

        return ((sb[1].sb_bunits == SLOB_PAGE_LIST_END)
            ? NULL
            : (sp->sp_page + (uintptr_t)sb[1].sb_bunits));
}

/*
 * Return true if the given block is at the tail of the block linked
 * list. Otherwise, return false.
 */
static bool
slob_block_list_last(struct slob_block *sb)
{
        if (sb == NULL) {
                return true;
        }

        return ((sb->sb_bunits == 0) && ((slob_block_list_next(sb)) == NULL));
}

/*
 * Attempt to allocate a block that is as least as big as BUNITS from a
 * SLOB page SP by first performing the next-fit method.
 *
 * If successful, a pointer to that block is returned. Otherwise, a NULL
 * pointer is returned if there is no block big enough to hold the
 * allocation request.
 */
static struct slob_block *
slob_block_alloc(struct slob_page *sp, uint32_t bunits)
{
        struct slob_block *block;
        struct slob_block *bprev;
        struct slob_block *bnext;

        uint32_t bavail;

        bprev = NULL;
        bnext = NULL;

        /* Walk the free block list starting at the first free block
         * (next-fit method) */
        block = sp->sp_meta.spm_bfree;

        bavail = 0;

        for (; ; bprev = block, block = slob_block_list_next(block)) {
                if (slob_block_list_last(block)) {
                        return NULL;
                }

                bavail = slob_block_units(block);

                if (bunits <= bavail) {
                        break;
                }
        }

        bnext = slob_block_list_next(block);

        if (bunits == bavail) {
                /* No fragmentation (exact fit) */

                if (bprev != NULL) {
                        slob_block_list_set(bprev, bnext, slob_block_units(bprev));
                } else {
                        sp->sp_meta.spm_bfree = slob_block_list_next(block);
                }
        } else if (bunits < bavail) {
                /* Fragmentation */

                /* Make a new free block */
                slob_block_list_set(block + bunits, bnext, bavail - bunits);

                if (bprev != NULL) {
                        /* Have the previous block point to the newly
                         * created block */
                        slob_block_list_set(bprev, block + bunits,
                            slob_block_units(bprev));
                } else {
                        sp->sp_meta.spm_bfree = block + bunits;
                }
        }

        sp->sp_meta.spm_bunits -= bunits;

        /* Record the number of units in the block */
        block->sb_bunits = bunits;

        return block;
}

/*
 * Return the the size of SB.
 */
static uint32_t
slob_block_units(struct slob_block *sb)
{
        return sb->sb_bunits;
}

/*
 * Given a pointer, return the the size of SB.
 */
static uint32_t
slob_block_units_ptr(void *addr)
{
        struct slob_block *sb;
        sb = (struct slob_block *)addr - 1;

        return slob_block_units(sb);
}

/*
 * Returns the SLOB page list that the requested allocation (in block
 * units) should be allocated to.
 */
static struct slob_page_list *
slob_page_list_select(uint32_t bunits)
{
        struct slob_page_list *spl;

        if (bunits <= SLOB_BLOCK_UNITS(SLOB_PAGE_BREAK_1ST)) {
                spl = &slob_small_page_list;
        } else if (bunits <= SLOB_BLOCK_UNITS(SLOB_PAGE_BREAK_2ND)) {
                spl = &slob_medium_page_list;
        } else {
                spl = &slob_large_page_list;
        }

        return spl;
}

/*-
 * Return number of pages allocated to the SLOB page list.
 *
 * If successful, the number of pages allocated to SPL is
 * returned. Otherwise a -1 is returned for the following cases:
 *
 *   - SPL is NULL
 */
static uint32_t
slob_page_list_count(struct slob_page_list *spl)
{
        if (spl == NULL) {
                return -1;
        }

        if (SLIST_EMPTY(spl)) {
                return 0;
        }

        uint32_t page_count;
        page_count = 0;

        struct slob_page *sp;
        sp = NULL;

        SLIST_FOREACH(sp, spl, sp_list) {
                page_count++;
        }

        return page_count;
}

/*-
 * Return number of free block units in SLOB page list.
 */
static uint32_t
slob_page_list_block_units(struct slob_page_list *spl)
{
        if (spl == NULL) {
                return 0;
        }

        if (SLIST_EMPTY(spl)) {
                return 0;
        }

        uint32_t bavail;
        bavail = 0;

        struct slob_page *sp;
        sp = NULL;

        SLIST_FOREACH(sp, spl, sp_list) {
                struct slob_block *block;
                block = sp->sp_meta.spm_bfree;

                /* Walk the free block list starting at the first free
                 * block */
                while (true) {
                        if (slob_block_list_last(block)) {
                                return bavail;
                        }

                        bavail += slob_block_units(block);

                        block = slob_block_list_next(block);
                }
        }

        return bavail;
}

/*-
 * Allocate, initialize, and append a new SLOB page onto the SLOB page
 * list SPL.
 *
 * If successful, 0 is returned. Otherwise a -1 is returned for the
 * following cases:
 *
 *   - An invalid SLOB page list was passed
 *   - Exceeded the total amount of SLOB pages
 */
static int
slob_page_alloc(struct slob_page_list *spl)
{
        struct slob_page *sp;
        struct slob_block *bbase;
        struct slob_block *blast;

        if (spl == NULL) {
                return -1;
        }

        if ((sp = (struct slob_page *)memb_alloc(&slob_pages)) == NULL) {
                return -1;
        }

        bbase = &sp->sp_page[0];

        sp->sp_meta.spm_bunits = SLOB_BLOCK_UNITS(SLOB_PAGE_ACTUAL_SIZE);
        sp->sp_meta.spm_bfree = bbase;

        /* Initialize free block linked list */
        blast = bbase + SLOB_BLOCK_UNITS(SLOB_PAGE_ACTUAL_SIZE);

        slob_block_list_set(bbase, blast, SLOB_BLOCK_UNITS(SLOB_PAGE_ACTUAL_SIZE));
        slob_block_list_term(blast, 0x0000000);

        SLIST_INSERT_HEAD(spl, sp, sp_list);

        return 0;
}

/*-
 * Remove a page SP from a SLOB page list SPL.
 *
 * If successful, 0 is returned. Otherwise a -1 is returned for the
 * following cases:
 *
 *   - An invalid SLOB page list SPL or SLOB page SP was passed
 *   - The SLOB page still has referenced/allocated blocks (less than the
 *     number of block units in a SLOB page)
 *   - The MEMB allocator wasn't able to free memory reference to SPL
 */
static int
slob_page_free(struct slob_page_list *spl, struct slob_page *sp)
{
        if (spl == NULL) {
                return -1;
        }

        if (sp == NULL) {
                return -1;
        }

        if (sp->sp_meta.spm_bunits != SLOB_BLOCK_UNITS(SLOB_PAGE_ACTUAL_SIZE)) {
                return -1;
        }

        if (!SLIST_EMPTY(spl)) {
                SLIST_REMOVE(spl, sp, slob_page, sp_list);
        }

        if ((memb_free(&slob_pages, sp)) < 0) {
                return -1;
        }

        return 0;
}
