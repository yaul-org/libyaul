/*
 * Copyright (c) 2011 Matthew Conte
 * See LICENSE for details.
 *
 * Matthew Conte <matt@baisoku.org>
 */

#ifndef _TLSF_H_
#define _TLSF_H_

#include <stddef.h>

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

typedef void (*tlsf_walker)(void *, size_t, int, void *);
typedef void *tlsf_pool;

tlsf_pool tlsf_create(void *, size_t);
void tlsf_destroy(tlsf_pool);

void *tlsf_malloc(tlsf_pool pool, size_t bytes);
void *tlsf_memalign(tlsf_pool pool, size_t align, size_t bytes);
void *tlsf_realloc(tlsf_pool pool, void* ptr, size_t size);
void tlsf_free(tlsf_pool pool, void* ptr);

void tlsf_walk_heap(tlsf_pool, tlsf_walker, void *);
int tlsf_check_heap(tlsf_pool);

size_t tlsf_block_size(void *);
size_t tlsf_overhead(void);

#if defined(__cplusplus)
};
#endif /* __cplusplus */

#endif /* !_TLSF_H_ */
