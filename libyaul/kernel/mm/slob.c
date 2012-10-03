/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <sys/queue.h>

#include <inttypes.h>

#include <lib/memb.h>
#include "slob.h"

STATIC_ASSERT(SLOB_PAGE_SIZE >= SLOB_PAGE_BREAK_2ND);

#define SLOB_BLOCK_UNIT         (sizeof(struct slob_block))
#define SLOB_BLOCK_UNITS(s)     (((s) + SLOB_BLOCK_UNIT - 1) / SLOB_BLOCK_UNIT)

struct slob_block {
        union {
                int16_t sb_bunits;
                /* Next "member" is a link to the next block relative to the
                 * starting address of the SLOB page */
                int16_t sb_bnext;
        } s;
};

struct slob_page {
        /* Page must be the first member in order to be aligned on a
         * SLOB page-boundry */
        struct slob_block sp_page[SLOB_BLOCK_UNITS(SLOB_PAGE_SIZE) + 1];
        int16_t sp_bunits; /* Free number of block units */
        struct slob_block *sp_bfree; /* Pointer to first free block */

        TAILQ_ENTRY(slob_page) sp_list; /* Link to next page */
} __attribute__ ((aligned(SLOB_PAGE_SIZE)));

TAILQ_HEAD(slob_page_list, slob_page);

static struct slob_block *slob_block_alloc(struct slob_page *, int16_t);
static int16_t slob_block_units(struct slob_block *);

static int slob_page_alloc(struct slob_page_list *);

static void slob_block_list_set(struct slob_block *, struct slob_block *, int16_t);
static struct slob_block *slob_block_list_next(struct slob_block *);
static int slob_block_list_last(struct slob_block *) __attribute__ ((unused));

static struct slob_page_list
        slob_small_page = TAILQ_HEAD_INITIALIZER(slob_small_page);
static struct slob_page_list
        slob_medium_page = TAILQ_HEAD_INITIALIZER(slob_medium_page);
static struct slob_page_list
        slob_large_page = TAILQ_HEAD_INITIALIZER(slob_large_page);

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

        int16_t bunits;
        int16_t btotal;

        bunits = SLOB_BLOCK_UNITS(size);
        btotal = SLOB_BLOCK_UNITS(SLOB_PAGE_SIZE);

        /* See restrictions */
        if (bunits > btotal)
                return NULL;

        /* Choose the right list of SLOB pages */
        if (size < SLOB_PAGE_BREAK_1ST)
                spl = &slob_small_page;
        else if (size < SLOB_PAGE_BREAK_2ND)
                spl = &slob_medium_page;
        else
                spl = &slob_large_page;

        block = NULL;
        TAILQ_FOREACH(sp, spl, sp_list) {
                /* If the request is too large, there is no need to walk
                 * the free block list */
                if (bunits > sp->sp_bunits)
                        continue;

                if ((block = slob_block_alloc(sp, bunits)) != NULL)
                        break;
        }

        if (block == NULL) {
                /* There is no SLOB page that can satisfy the request
                 * allocation. */
                if ((slob_page_alloc(spl)) < 0) {
                        /* Completely exhausted pool of SLOB pages */
                        return NULL;
                }
                /* Okay, let's try again */
                sp = TAILQ_FIRST(spl);
                block = slob_block_alloc(sp, bunits);
        }

        return block;
}

/*
 *
 */
void
slob_free(void *addr __attribute__ ((unused)))
{
        /* IMPLEMENT-ME */
}

/*
 * Helper function(s) for the free block linked list within a SLOB page.
 */

/*
 * Link two SLOB blocks together. Namely, have SB link to SB_NEXT.
 */
static void
slob_block_list_set(struct slob_block *sb, struct slob_block *sb_next,
    int16_t units)
{
        struct slob_block *bbase;
        int16_t bofs;

        bbase = (struct slob_block *)((uintptr_t)sb & SLOB_PAGE_MASK);
        bofs = sb_next - bbase;

        if (units <= 0)
                sb[0].s.sb_bunits = -bofs;
        else {
                sb[0].s.sb_bunits = units;

                sb[1].s.sb_bnext = bofs;
        }
}

/*
 * Return a pointer to the next free block in the free block linked
 * list.
 */
static struct slob_block *
slob_block_list_next(struct slob_block *sb)
{
        struct slob_block *bbase;
        uintptr_t bnext;

        bbase = (struct slob_block *)((uintptr_t)sb & SLOB_PAGE_MASK);
        if (sb[0].s.sb_bunits < 0)
                bnext = -sb[0].s.sb_bunits;
        else if (sb == (bbase + SLOB_BLOCK_UNITS(SLOB_PAGE_SIZE) - 1))
                bnext = 0;
        else
                bnext = sb[1].s.sb_bunits;

        return bbase + bnext;
}

/*
 * Return 1 if the given block is at the tail of the block linked
 * list. Otherwise, return 0.
 */
static int
slob_block_list_last(struct slob_block *sb)
{
        struct slob_block *bbase;

        bbase = (struct slob_block *)((uintptr_t)sb & SLOB_PAGE_MASK);

        return ((slob_block_list_next(sb)) == bbase);
}

/*
 *
 */
static struct slob_block *
slob_block_alloc(struct slob_page *sp, int16_t bunits)
{
        struct slob_block *block;
        struct slob_block *bprev;
        struct slob_block *bnext;

        uint16_t bavail;

        bprev = NULL;
        bnext = NULL;
        /* Walk the free block list starting at the first free block
         * (next-fit method) */
        block = sp->sp_bfree;
        for (; ; bprev = block, block = slob_block_list_next(block)) {
                bavail = slob_block_units(block);

                if (bunits > bavail)
                        continue;

                bnext = slob_block_list_next(block);
                if (bunits == bavail) {
                        /* No fragmentation (exact fit) */
                        if (bprev != NULL)
                                slob_block_list_set(bprev, bnext, bunits);
                        sp->sp_bfree = bnext;
                } else if (bunits < bavail) {
                        /* Fragmentation */
                        /* Make a new free block */
                        slob_block_list_set(block + bunits, bnext,
                            bavail - bunits);

                        if (bprev != NULL) {
                                /* Have the previous block point to the
                                 * newly created block */
                                slob_block_list_set(bprev, block + bunits,
                                    bprev->s.sb_bunits);
                        } else
                                sp->sp_bfree = block + bunits;
                }

                sp->sp_bunits -= bunits;
                return block;
        }

        return NULL;
}

/*
 * Return the the size of a given block.
 */
static int16_t
slob_block_units(struct slob_block *sb)
{
        return (sb->s.sb_bunits > 0) ? sb->s.sb_bunits : 1;
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

        if (spl == NULL)
                return -1;

        if ((sp = (struct slob_page *)memb_alloc(&slob_pages)) == NULL)
                return -1;

        bbase = &sp->sp_page[0];
        sp->sp_bunits = SLOB_BLOCK_UNITS(SLOB_PAGE_SIZE);
        sp->sp_bfree = bbase;
        /* Initialize free block linked list */
        blast = bbase + SLOB_BLOCK_UNITS(SLOB_PAGE_SIZE);
        slob_block_list_set(bbase, blast, SLOB_BLOCK_UNITS(SLOB_PAGE_SIZE));

        TAILQ_INSERT_HEAD(spl, sp, sp_list);

        return 0;
}
