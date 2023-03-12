/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

#include <internal.h>
#include <dbgio/dbgio-internal.h>

static inline uint8_t __always_inline
_1bpp_4bpp_convert(const uint8_t byte, const uint8_t *fgbg)
{
    const uint8_t ubyte = (byte >> 1) & 0x01;
    const uint8_t lbyte = byte & 0x01;

    return (fgbg[lbyte] << 4) | fgbg[ubyte];
}

void
__dbgio_font_1bpp_4bpp_decompress(const dbgio_font_t *font, uint8_t *dec_cg)
{
    assert(dec_cg != NULL);
    assert(((uintptr_t)font->cg & 0x00000003) == 0x00000000);

    const uint8_t fgbg[] = {
        font->bg & 0x0F,
        font->fg & 0x0F
    };

    for (uint32_t i = 0, j = 0; i < font->cg_size; i++) {
        uint8_t cg;
        cg = font->cg[i];

        dec_cg[j + 0] = _1bpp_4bpp_convert(cg, fgbg);
        cg >>= 2;
        dec_cg[j + 1] = _1bpp_4bpp_convert(cg, fgbg);
        cg >>= 2;
        dec_cg[j + 2] = _1bpp_4bpp_convert(cg, fgbg);
        cg >>= 2;
        dec_cg[j + 3] = _1bpp_4bpp_convert(cg, fgbg);

        j += 4;
    }
}
