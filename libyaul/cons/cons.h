/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#ifndef _CONS_H_
#define _CONS_H_

#include "vt_parse/vt_parse.h"

/* Half the normal size */
#define COLS            40
#define ROWS            28
#define TAB_WIDTH       2

struct cons {
        struct {
                uint16_t col;
                uint16_t row;
        } cursor;

        vt_parse_t vt_parser;

        void (*write)(struct cons *, int, uint8_t, uint8_t);
        void *driver;
};

void cons_write(struct cons *, const char *);

#endif /* !_CONS_H_ */
