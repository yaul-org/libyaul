/*
 * Copyright (c) 2022-2023 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include "internal.h"

static texture_t _default_texture[] = {
    {
        .vram_index = TEXTURE_VRAM_INDEX(0),
        .size       = TEXTURE_SIZE(0, 0)
    }
};

void
__tlist_init(void)
{
    list_t * const tlist = &__state.tlist->list;

    tlist->flags = LIST_FLAGS_NONE;
    tlist->buffer = _default_texture;
    tlist->count = 0;
    tlist->size = sizeof(texture_t);
    tlist->default_element = _default_texture;
}

texture_t *
tlist_acquire(uint32_t count)
{
    list_t * const list = &__state.tlist->list;

    __list_alloc(list, count);

    return list->buffer;
}

void
tlist_release(void)
{
    list_t * const list = &__state.tlist->list;

    __list_free(list);
}

texture_t *
tlist_get(void)
{
    return __state.tlist->list.buffer;
}

void
tlist_set(texture_t *textures, uint16_t count)
{
    list_t * const list = &__state.tlist->list;

    __list_set(list, textures, count);
}
