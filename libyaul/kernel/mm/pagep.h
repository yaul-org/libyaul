#ifndef _YAUL_KERNEL_MM_PAGEP_H_
#define _YAUL_KERNEL_MM_PAGEP_H_

#include <stdint.h>

#include <sys/cdefs.h>

#include <mm/memb.h>

typedef struct pagep {
    memb_t memb;
} pagep_t;

typedef struct pages {
    void *base;
    uint32_t count;
} pages_t;

static_assert(sizeof(pages_t) == 8);

void pagep_pool_hwram_alloc(pagep_t *pagep, uint32_t page_count, size_t page_size);
void pagep_pool_hwram_free(pagep_t *pagep);

void pagep_pool_init(pagep_t *pagep, void *base, uint32_t page_count, size_t page_size);

size_t pagep_page_count(const pagep_t *pagep, size_t byte_size);
pages_t pagep_byte_alloc(pagep_t *pagep, size_t byte_size);
pages_t pagep_page_alloc(pagep_t *pagep, uint32_t page_count);

void pagep_free(pagep_t *pagep, pages_t *pages);

#endif /* !_YAUL_KERNEL_MM_PAGEP_H_ */
