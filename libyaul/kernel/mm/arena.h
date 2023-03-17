#ifndef _YAUL_KERNEL_MM_ARENA_H_
#define _YAUL_KERNEL_MM_ARENA_H_

#include <stdint.h>
#include <stddef.h>

typedef struct {
    void *buffer;
    void *buffer_end;
    size_t buffer_size;

    void *buffer_offset;
} arena_t;

void arena_init(arena_t *arena, void *base, size_t byte_size);

void *arena_alloc(arena_t *arena, size_t byte_size);

void arena_reset(arena_t *arena);

#endif /* !_YAUL_KERNEL_MM_ARENA_H_ */
