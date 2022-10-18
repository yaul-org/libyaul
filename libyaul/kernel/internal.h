/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _KERNEL_INTERNAL_H_
#define _KERNEL_INTERNAL_H_

#include <stdint.h>
#include <math.h>

#include <sys/cdefs.h>

#include <mm/tlsf.h>

struct state {
        uint8_t which;

        /* Both master and slave contain their own set of handles */
        tlsf_t *tlsf_handles;
};

static inline struct state * __always_inline
master_state(void)
{
        extern struct state __master_state;

        return &__master_state;
}

static inline struct state * __always_inline
slave_state(void)
{
        extern struct state __slave_state;

        return &__slave_state;
}

extern void *_end;

typedef void *(*malloc_func_t)(size_t n);
typedef void *(*memalign_func_t)(size_t n, size_t align);
typedef void (*free_func_t)(void *p);

void __reset(void);

void __mm_init(void);

void *__malloc(size_t n);
void *__realloc(void *p, size_t n);
void *__memalign(size_t n, size_t align);
void __free(void *p);

void __atexit_init(void);

#endif /* !_KERNEL_INTERNAL_H_ */
