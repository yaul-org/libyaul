#include <assert.h>
#include <stdlib.h>

#include <gamemath/uint32.h>

#include "arena.h"

void
arena_init(arena_t *arena, void *base, size_t byte_size)
{
    assert(arena != NULL);
    assert(base != NULL);
    assert(byte_size > 0);

    arena->buffer_size = byte_size;
    arena->buffer = base;
    arena->buffer_end = (void *)((uintptr_t)arena->buffer + arena->buffer_size);

    arena_reset(arena);
}

void *
arena_alloc(arena_t *arena, size_t byte_size)
{
    assert(arena != NULL);

    const uintptr_t buffer_end = (uintptr_t)arena->buffer_end;
    const uintptr_t aligned_offset =
      uint32_pow2_round((uintptr_t)arena->buffer_offset, 4);

    if ((aligned_offset > buffer_end) ||
        (byte_size > (buffer_end - aligned_offset))) {
        return NULL;
    }

    arena->buffer_offset = (void *)(aligned_offset + byte_size);

    return (void *)aligned_offset;
}

void
arena_reset(arena_t *arena)
{
    assert(arena != NULL);

    arena->buffer_offset = arena->buffer;
}
