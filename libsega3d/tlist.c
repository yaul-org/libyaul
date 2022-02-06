/*
 * Copyright (c) 2020
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <stdlib.h>

#include "sega3d.h"

#include "sega3d-internal.h"

static TEXTURE _default_texture[] = {
        TEXDEF(0, 0, 0x00000000)
};

void
__tlist_init(void)
{
        list_t * const tlist = __state->tlist;

        tlist->flags = LIST_FLAGS_NONE;
        tlist->list = _default_texture;
        tlist->count = 0;
        tlist->size = sizeof(TEXTURE);
        tlist->default_element = _default_texture;
}

TEXTURE *
sega3d_tlist_alloc(uint16_t texture_count)
{
        __list_alloc(__state->tlist, texture_count);

        return __state->tlist->list;
}

void
sega3d_tlist_free(void)
{
        __list_free(__state->tlist);
}

void
sega3d_tlist_set(TEXTURE *textures, uint16_t texture_count)
{
        __list_set(__state->tlist, textures, texture_count);
}
