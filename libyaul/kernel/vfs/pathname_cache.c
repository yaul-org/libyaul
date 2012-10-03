/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include "hash.h"
#include "component_name.h"
#include "vnode.h"

#include <lib/memb.h>

struct pathname_cache_entry {
        const struct component_name *pce_cnp;
        struct vnode *pce_vnp;
        uint32_t pce_refcnt; /* Reference count for LRU management */

        struct hash_table_handle pce_hth;
};

HASH(pathname_cache, struct pathname_cache_entry);
MEMB(pathname_cache_mb, struct pathname_cache_entry,
    HASH_CAPACITY_COUNT(), 0);

/*
 * Initialize the pathname cache.
 */ 
void
pathname_cache_init(void)
{
        /* Initialize block pool */
        memb_init(&pathname_cache_mb);
}

/*-
 * Enter an entry into the cache.
 *
 * If successful, 0 is returned. Otherwise a -1 is returned for the
 * following cases:
 *
 *   - Adding an entry into the pathname results in a collision (conflict
 *     miss);
 *   - No entries available (capacity miss);
 *   - Length of pathname is too long; or
 *   - Bad type of virtual-node
 */
int
pathname_cache_enter(const struct component_name *cnp, struct vnode *vnp)
{
        struct pathname_cache_entry *entry;

        /* Sanitize arguments */
        if ((cnp == NULL) || (vnp == NULL))
                return -1;
        if ((vnp->vn_type == VNODE_OBJ_NONE) ||
            (vnp->vn_type == VNODE_OBJ_BAD))
                return -1;
        /* Check component name */

        if ((entry = memb_alloc(&pathname_cache_mb)) == NULL)
                return -1;

        /* Check if the entry allocated from the block pool has a
         * reference count of zero. */
        if (entry->pce_refcnt > 0)
                return -1;

        entry->pce_cnp = cnp;
        entry->pce_vnp = vnp;
        /* Set first reference */
        entry->pce_refcnt = 1;

        HASH_ENTER_PTR(pce_hth, pathname_cache, pce_cnp, entry);

        return 0;
}

/*-
 * Resolve a pathname and find the corresponding virtual-node in cache.
 * 
 * If successful, 0 is returned. Otherwise a -1 is returned for the
 * following cases:
 *
 *   - Non-existant pathname (cache miss) or
 *   - Length of pathname is too long
 */
int
pathname_cache_lookup(const struct component_name *cnp __attribute__ ((unused)),
    struct vnode **vnp __attribute__ ((unused)))
{
        return -1;
}

/*-
 * Evict a (pathname, virtual-node) pair from cache.
 * 
 * If successful, 0 is returned. Otherwise a -1 is returned if there is
 * a non-existant pathname (cache miss).
 */
int
pathname_cache_evict(const struct component_name *cnp __attribute__ ((unused)))
{
        return 0;
}

/*
 * Evict the least recently used pathname entry in cache.
 */
void
pathname_cache_evict_lru(void)
{
}
