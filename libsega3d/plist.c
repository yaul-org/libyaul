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
_internal_plist_init(void)
{
        list_t * const plist = _internal_state->plist;

        plist->flags = LIST_FLAGS_NONE;
        plist->list = NULL;
        plist->count = 0;
        plist->size = sizeof(PALETTE);
        plist->default_element = NULL;
}

PALETTE *
sega3d_plist_alloc(uint16_t palette_count)
{
        internal_list_alloc(_internal_state->plist, palette_count);

        return _internal_state->plist->list;
}

void
sega3d_plist_free(void)
{
        internal_list_free(_internal_state->plist);
}

void
sega3d_plist_set(PALETTE *palettes, uint16_t palette_count)
{
        internal_list_set(_internal_state->plist, palettes, palette_count);
}
