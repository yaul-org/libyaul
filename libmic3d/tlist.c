/*
 * Copyright (c) 2022-2023 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include "internal.h"

void
__tlist_init(void)
{
    tlist_t * const tlist = __state.tlist;

    tlist->textures = NULL;
    tlist->texture_count = 0;
}

const texture_t *
tlist_get(void)
{
    tlist_t * const tlist = __state.tlist;

    return tlist->textures;
}

void
tlist_set(const texture_t *textures, uint16_t count)
{
    tlist_t * const tlist = __state.tlist;

    tlist->textures = textures;
    tlist->texture_count = (textures != NULL) ? count : 0;
}
