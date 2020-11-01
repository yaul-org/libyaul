/*
 * Copyright (c) 2020
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>
#include <stdlib.h>

#include <sys/cdefs.h>

#include "sega3d.h"

typedef enum {
        TLIST_FLAGS_NONE,
        TLIST_FLAGS_USER_ALLOCATED = 1 << 0,
        TLIST_FLAGS_ALLOCATED      = 1 << 1
} tlist_flags_t;

typedef struct {
        tlist_flags_t flags;
        TEXTURE *textures;
        uint16_t count;
        uint16_t cursor;
} tlist_t;

static tlist_t *_tlist;

static TEXTURE _default_texture[] = {
        TEXDEF(0, 0, 0x00000000)
};

static POINT point_PLANE1[] = {
        POStoFIXED(-10.0, -10.0, 0.0),
        POStoFIXED( 10.0, -10.0, 0.0),
        POStoFIXED( 10.0,  10.0, 0.0),
        POStoFIXED(-10.0,  10.0, 0.0),
};

static POLYGON polygon_PLANE1[] = {
        NORMAL(0.0, 1.0, 0.0), VERTICES(0, 1, 2, 3)
};

static ATTR attribute_PLANE1[] = {
        ATTRIBUTE(Dual_Plane, SORT_CEN, No_Texture, C_RGB(31, 31, 31), No_Gouraud, MESHoff, sprPolygon, No_Option)
};

PDATA PD_PLANE1 = {
        point_PLANE1,
        sizeof(point_PLANE1) / sizeof(POINT),
        polygon_PLANE1,
        sizeof(polygon_PLANE1) / sizeof(POLYGON),
        attribute_PLANE1
};

void
sega3d_tlist_alloc(uint16_t count)
{
        assert(count > 0);

        if (_tlist->count >= count) {
                return;
        }

        _tlist->textures = realloc(_tlist->textures, sizeof(TEXTURE) * count);
        assert(_tlist->textures);

        _tlist->count = count;

        _tlist->flags |= TLIST_FLAGS_ALLOCATED;
}

void
sega3d_tlist_free(void)
{
        sega3d_tlist_set(NULL, 0);
}

void
sega3d_tlist_set(TEXTURE *textures, uint16_t count)
{
        if ((_tlist->flags & TLIST_FLAGS_ALLOCATED) == TLIST_FLAGS_ALLOCATED) {
                free(_tlist->textures);

                _tlist->flags &= ~TLIST_FLAGS_ALLOCATED;
        }

        if ((textures != NULL) && (count != 0)) {
                _tlist->textures = textures;
                _tlist->count = count;

                _tlist->flags |= TLIST_FLAGS_USER_ALLOCATED;
        } else {
                _tlist->textures = _default_texture;
                _tlist->count = 0;

                _tlist->flags &= ~TLIST_FLAGS_USER_ALLOCATED;
        }
}

TEXTURE *
sega3d_tlist_tex_append(void)
{
        assert(_tlist->textures != NULL);
        assert(_tlist->count != 0);

        uint32_t next_cursor;
        next_cursor = (uint32_t)_tlist->cursor + 1;

        assert(next_cursor < _tlist->count);

        TEXTURE *next_texture;
        next_texture = sega3d_tlist_tex_get(next_cursor); 

        _tlist->count = next_cursor;

        return next_texture;
}

TEXTURE *
sega3d_tlist_tex_get(uint16_t cursor)
{
        assert(_tlist->textures != NULL);
        assert(_tlist->count != 0);

        assert(cursor < _tlist->count);

        TEXTURE *texture;
        texture = &_tlist->textures[cursor];

        return texture;
}

uint16_t
sega3d_tlist_cursor_get(void)
{
        return _tlist->cursor;
}

void
sega3d_tlist_cursor_reset(void)
{
        _tlist->cursor = 0;
}

uint16_t
sega3d_polycount_get(const PDATA *pdata)
{
        return pdata->nbPolygon;
}

void
sega3d_cmdt_prepare(const PDATA *pdata, vdp1_cmdt_list_t *cmdt_list)
{
        assert(pdata != NULL);
        assert(cmdt_list != NULL);
        assert(cmdt_list->cmdts != NULL);
        /* Make sure we have enough */
        assert(cmdt_list->count >= pdata->nbPolygon);

        for (uint32_t i = 0; i < cmdt_list->count; i++) {
                vdp1_cmdt_t *cmdt;
                cmdt = &cmdt_list->cmdts[i];

                const ATTR *attr;
                attr = &pdata->attbl[i];
                
                cmdt->cmd_ctrl = attr->dir; /* We care about (Dir) and (Comm) bits */
                cmdt->cmd_link = 0x0000;
                cmdt->cmd_pmod = attr->atrb;
                cmdt->cmd_colr = attr->colno;

                /* Even when there is not texture list, there is the default
                 * texture that zeroes out CMDSRCA and CMDSIZE */
                const TEXTURE *texture;
                texture = sega3d_tlist_tex_get(attr->texno);

#ifdef DEBUG
                /* If the texture number is zero, it could imply no texture.
                 * Even if the texture list is empty, it's considered valid */
                if (attr->texno != 0) {
                        assert(attr->texno < _tlist->count);
                }
#endif /* DEBUG */

                cmdt->cmd_srca = texture->CGadr;
                cmdt->cmd_size = texture->HVsize;
        }
}
