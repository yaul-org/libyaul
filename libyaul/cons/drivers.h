/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _DRIVERS_H_
#define _DRIVERS_H_

#include <vdp2.h>

#include "cons.h"

#define FONT_H          8
#define FONT_W          8

#define FONT_NCOLORS    16

struct cons_buffer;

struct cons {
        struct {
                int32_t col;
                int32_t row;
        } cursor;

        bool initialized;

        void (*write)(struct cons *);

        struct cons_buffer *buffer;
};

struct cons_buffer {
        uint8_t glyph;
        uint8_t attribute;
        uint8_t fg;
        uint8_t bg;
};

void cons_vdp1_init(struct cons *);
void cons_vdp2_init(struct cons *);

extern const uint32_t font[];
extern const uint16_t palette[];

#endif /* !_DRIVERS_H_ */
