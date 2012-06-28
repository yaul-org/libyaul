/*
 * Copyright (c) 2011 Matthew Conte
 * See LICENSE for details.
 *
 * Matthew Conte <matt@baisoku.org>
 */

#ifndef _TLSF_H_
#define _TLSF_H_

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef void (*tlsf_walker)(void *, size_t, int, void *);
typedef void *tlsf_pool;

int tlsf_check_heap(tlsf_pool);
size_t tlsf_block_size(void *);
size_t tlsf_overhead(void);
tlsf_pool tlsf_create(void *, size_t);
void *tlsf_malloc(tlsf_pool, size_t);
void *tlsf_memalign(tlsf_pool, size_t, size_t);
void *tlsf_realloc(tlsf_pool, void *, size_t);
void tlsf_destroy(tlsf_pool);
void tlsf_free(tlsf_pool, void *);
void tlsf_walk_heap(tlsf_pool, tlsf_walker, void *);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* !_TLSF_H_ */
