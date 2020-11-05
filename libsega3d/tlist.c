#include <stdlib.h>

#include "sega3d.h"

#include "sega3d-internal.h"

typedef struct {
        tlist_flags_t flags;
        TEXTURE *textures;
        uint16_t count;
        uint16_t cursor;
} tlist_t;

static tlist_t _tlist;

static TEXTURE _default_texture[] = {
        TEXDEF(0, 0, 0x00000000)
};

void
_internal_tlist_init(void)
{
        _tlist.flags = TLIST_FLAGS_NONE;

        sega3d_tlist_cursor_reset();

        sega3d_tlist_set(NULL, 0);
}

void
sega3d_tlist_alloc(uint16_t count)
{
        assert(count > 0);

        if (_tlist.count >= count) {
                return;
        }

        _tlist.textures = realloc(_tlist.textures, sizeof(TEXTURE) * count);
        assert(_tlist.textures);

        _tlist.count = count;

        _tlist.flags |= TLIST_FLAGS_ALLOCATED;
}

void
sega3d_tlist_free(void)
{
        sega3d_tlist_set(NULL, 0);
}

void
sega3d_tlist_set(TEXTURE *textures, uint16_t count)
{
        if ((_tlist.flags & TLIST_FLAGS_ALLOCATED) == TLIST_FLAGS_ALLOCATED) {
                free(_tlist.textures);

                _tlist.flags &= ~TLIST_FLAGS_ALLOCATED;
        }

        if ((textures != NULL) && (count != 0)) {
                _tlist.textures = textures;
                _tlist.count = count;

                _tlist.flags |= TLIST_FLAGS_USER_ALLOCATED;
        } else {
                _tlist.textures = _default_texture;
                _tlist.count = 0;

                _tlist.flags &= ~TLIST_FLAGS_USER_ALLOCATED;
        }
}

uint16_t
sega3d_tlist_count_get(void)
{
        return _tlist.count;
}

TEXTURE *
sega3d_tlist_tex_append(void)
{
        assert(_tlist.textures != NULL);
        assert(_tlist.count != 0);

        uint32_t next_cursor;
        next_cursor = (uint32_t)_tlist.cursor + 1;

        assert(next_cursor < _tlist.count);

        TEXTURE *next_texture;
        next_texture = sega3d_tlist_tex_get(next_cursor); 

        _tlist.count = next_cursor;

        return next_texture;
}

TEXTURE *
sega3d_tlist_tex_get(uint16_t cursor)
{
        if (_tlist.count == 0) {
                return NULL;
        }

        if (_tlist.textures == NULL) {
                return NULL;
        }

        assert(cursor < _tlist.count);

        TEXTURE *texture;
        texture = &_tlist.textures[cursor];

        return texture;
}

uint16_t
sega3d_tlist_cursor_get(void)
{
        return _tlist.cursor;
}

void
sega3d_tlist_cursor_reset(void)
{
        _tlist.cursor = 0;
}
