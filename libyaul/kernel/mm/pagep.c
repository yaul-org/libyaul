#include <cpu/divu.h>

#include <scu/map.h>

#include <assert.h>
#include <stdio.h>

#include "pagep.h"

void
pagep_pool_hwram_alloc(pagep_t *pagep, uint32_t page_count, size_t page_size)
{
    assert(pagep != NULL);

    assert(page_count > 0);

    /* Assert that page size is POW2 */
    assert((page_size & (page_size - 1)) == 0);

    const int32_t ret __unused =
      memb_memb_alloc(&pagep->memb, page_count, page_size, page_size);
    assert(ret == 0);
}

void
pagep_pool_hwram_free(pagep_t *pagep)
{
    assert(pagep != NULL);

    memb_memb_free(&pagep->memb);
}

void
pagep_pool_init(pagep_t *pagep, void *base, uint32_t page_count, size_t page_size)
{
    assert(pagep != NULL);

    assert(page_count > 0);

    /* Assert that page size is POW2 */
    assert((page_size & (page_size - 1)) == 0);

    assert(base != NULL);

    /* Assert that base is on a page size boundary */
    assert(((uintptr_t)base & (page_size - 1)) == 0);

    const int32_t ret __unused =
      memb_memb_init(&pagep->memb, base, page_count, page_size);
    assert(ret == 0);
}

size_t
pagep_page_count(const pagep_t *pagep, size_t byte_size)
{
    assert(pagep != NULL);

    cpu_divu_32_32_set(byte_size + ((pagep->memb.size) - 1), pagep->memb.size);

    return cpu_divu_quotient_get();
}

pages_t
pagep_byte_alloc(pagep_t *pagep, size_t byte_size)
{
    assert(pagep != NULL);

    return pagep_page_alloc(pagep, pagep_page_count(pagep, byte_size));
}

pages_t
pagep_page_alloc(pagep_t *pagep, uint32_t page_count)
{
    assert(pagep != NULL);

    pages_t pages;

    pages.base = memb_contiguous_alloc(&pagep->memb, page_count);
    pages.count = page_count;

    if (pages.base == NULL) {
        pages.count = 0;
    }

    return pages;
}

void
pagep_free(pagep_t *pagep, pages_t *pages)
{
    assert(pagep != NULL);

    memb_free(&pagep->memb, pages->base);

    pages->base = NULL;
    pages->count = 0;
}
