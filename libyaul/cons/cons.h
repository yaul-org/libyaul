/*
 * Copyright (c) 2012 Israel Jacquez
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

struct cons {
        struct {
                int32_t col;
                int32_t row;
        } cursor;

        vt_parse_t vt_parser;

        void (*clear)(struct cons *, int32_t, int32_t, int32_t, int32_t);
        void (*reset)(struct cons *);
        void (*scroll)(struct cons *);
        void (*write)(struct cons *, int, uint8_t, uint8_t);
        void *driver;
};

void cons_write(struct cons *, const char *);
void cons_reset(struct cons *);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* !_CONS_H_ */
