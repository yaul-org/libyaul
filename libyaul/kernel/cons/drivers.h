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

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define FONT_CHAR_HEIGHT        (8)
#define FONT_CHAR_WIDTH         (8)
#define FONT_SIZE               (8192)
#define FONT_COLOR_COUNT        (16)

struct cons_buffer;

struct cons {
        uint16_t cols;
        uint16_t rows;
        uint16_t cell_count;

        struct {
                int32_t col;
                int32_t row;
        } cursor;

        void (*write)(struct cons *);

        struct cons_buffer *buffer;
};

struct cons_buffer {
        uint8_t glyph;
} __packed;

void cons_vdp1_init(struct cons *);
void cons_vdp2_init(struct cons *);
void cons_usb_cartridge_init(struct cons *);

extern const uint32_t font[];
extern const uint16_t palette[];

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !_DRIVERS_H_ */
