#include <assert.h>
#include <stdlib.h>
#include <math.h>

#include "arena.h"

void
arena_init(arena_t *arena, void *base, size_t byte_size)
{
    assert(arena != NULL);
    assert(base != NULL);
    assert(byte_size > 0);

    arena->buffer_size = uint32_pow2_round(byte_size, 4);
    arena->buffer = base;
    arena->buffer_end = (void *)((uintptr_t)arena->buffer + arena->buffer_size);

    arena_reset(arena);
}

void *
arena_alloc(arena_t *arena, size_t byte_size)
{
    assert(arena != NULL);

    const uintptr_t buffer_pos = (uintptr_t)arena->buffer_offset + byte_size;
    const uintptr_t aligned_buffer_pos = uint32_pow2_round(buffer_pos, 4);

    if (aligned_buffer_pos >= (uintptr_t)arena->buffer_end) {
        return NULL;
    }

    const uintptr_t buffer_offset = (uintptr_t)arena->buffer_offset;

    arena->buffer_offset = (void *)(buffer_offset + byte_size + (aligned_buffer_pos - buffer_pos));

    return (void *)aligned_buffer_pos;
}

void
arena_reset(arena_t *arena)
{
    assert(arena != NULL);

    arena->buffer_offset = arena->buffer;
}
