/*
 * Copyright (c) 2020
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <stdlib.h>

#include "sega3d.h"

#include "sega3d-internal.h"

void
__plist_init(void)
{
        list_t * const plist = __state->plist;

        plist->flags = LIST_FLAGS_NONE;
        plist->list = NULL;
        plist->count = 0;
        plist->size = sizeof(PALETTE);
        plist->default_element = NULL;
}

PALETTE *
sega3d_plist_alloc(uint16_t palette_count)
{
        __list_alloc(__state->plist, palette_count);

        return __state->plist->list;
}

void
sega3d_plist_free(void)
{
        __list_free(__state->plist);
}

void
sega3d_plist_set(PALETTE *palettes, uint16_t palette_count)
{
        __list_set(__state->plist, palettes, palette_count);
}
