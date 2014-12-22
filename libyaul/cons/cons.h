/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _CONS_H_
#define _CONS_H_

#include <inttypes.h>

#include "font.h"
#include "vt_parse/vt_parse.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Half the normal size */
#define COLS            40
#define ROWS            28
#define TAB_WIDTH       2

struct cons_buffer;

struct cons {
        struct {
                int32_t col;
                int32_t row;
        } cursor;

        vt_parse_t vt_parser;

        void (*write)(struct cons *);

        struct cons_buffer *buffer;
};

struct cons_buffer {
        uint8_t glyph;
};

#define CONS_DRIVER_VDP1        0
#define CONS_DRIVER_VDP2        1

extern void cons_init(struct cons *, uint8_t);
extern void cons_buffer(struct cons *, const char *);
extern void cons_flush(struct cons *);
extern void cons_write(struct cons *, const char *);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* !_CONS_H_ */
