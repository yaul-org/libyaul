/*
 * Copyright (c) Israel Jacquez
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
    /* Must be on a 4-byte boundary */
    assert(((uintptr_t)dec_cg & 0x00000003) == 0x00000000);
    /* Must be on a 4-byte boundary */
    assert(((uintptr_t)font->cg & 0x00000003) == 0x00000000);

    /* The '\0' character will always have 0/0 FG/BG palette indices,
     * respectively */
    uint32_t *clear_cg;
    clear_cg = (uint32_t *)dec_cg;

    *clear_cg++ = 0x00000000;
    *clear_cg++ = 0x00000000;
    *clear_cg++ = 0x00000000;
    *clear_cg++ = 0x00000000;
    *clear_cg++ = 0x00000000;
    *clear_cg++ = 0x00000000;
    *clear_cg++ = 0x00000000;
    *clear_cg++ = 0x00000000;

    dec_cg = (uint8_t *)clear_cg;

    const uint8_t fgbg[] = {
        font->bg_pal & 15,
        font->fg_pal & 15
    };

    /* Skip the first 1-BPP character (8 bytes) */
    const uint32_t offset_1bpp = '\x01' * 8;

    for (uint32_t i = offset_1bpp; i < font->cg_size; i++) {
        uint8_t cg;
        cg = font->cg[i];

        *dec_cg++ = _1bpp_4bpp_convert(cg, fgbg);
        cg >>= 2;
        *dec_cg++ = _1bpp_4bpp_convert(cg, fgbg);
        cg >>= 2;
        *dec_cg++ = _1bpp_4bpp_convert(cg, fgbg);
        cg >>= 2;
        *dec_cg++ = _1bpp_4bpp_convert(cg, fgbg);
    }
}
