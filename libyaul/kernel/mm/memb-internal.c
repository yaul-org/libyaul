#include "memb-internal.h"

int
__memb_memb_request_init(memb_t *memb, void *pool,
    const memb_request_t *request)
{
        assert(memb != NULL);
        assert(pool != NULL);

        assert(request->block_count != 0);
        assert(request->block_size != 0);

        const size_t ref_size = sizeof(memb_ref_t) * request->block_count;
        memb_ref_t * const refs = request->malloc_func(ref_size);

        if (refs == NULL) {
                return -1;
        }

        memb->type = MEMB_TYPE_SET;
        memb->size = request->block_size;
        memb->count = request->block_count;
        memb->refs = refs;
        memb->pool = pool;

        memb_init(memb);

        return 0;
}

int
__memb_memb_request_alloc(memb_t *memb, const memb_request_t *request)
{
        assert(memb != NULL);

        assert(request->block_count != 0);
        assert(request->block_size != 0);

        const uint32_t align = max(request->align, 4UL);

        const size_t ref_size = sizeof(memb_ref_t) * request->block_count;
        const size_t pool_size = request->block_size * request->block_count;

        /* Allocate all memory needed in a single request */
        void * const area = request->memalign_func(ref_size + pool_size, align);

        if (area == NULL) {
                return -1;
        }

        uintptr_t area_ptr = (uintptr_t)area;

        memb->type = MEMB_TYPE_DYNAMIC;
        memb->size = request->block_size;
        memb->count = request->block_count;
        memb->refs = (void *)(area_ptr + pool_size);
        /* Have the pool be at the top of the allocation request for
         * alignment */
        memb->pool = (void *)area_ptr;

        memb->free = request->free_func;

        memb_init(memb);

        return 0;
}

int
__memb_memb_init(memb_t *memb, void *pool, uint32_t block_count,
    uint32_t block_size)
{
        const memb_request_t request = {
                .malloc_func = __malloc,
                .free_func   = __free,
                .block_count = block_count,
                .block_size  = block_size
        };

        return __memb_memb_request_init(memb, pool, &request);
}

int
__memb_memb_alloc(memb_t *memb, uint32_t block_count, uint32_t block_size,
    uint32_t align)
{
        const memb_request_t request = {
                .malloc_func   = __malloc,
                .memalign_func = __memalign,
                .free_func     = __free,
                .block_count   = block_count,
                .block_size    = block_size,
                .align         = align
        };

        return __memb_memb_request_alloc(memb, &request);
}
