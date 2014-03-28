/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <inttypes.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

#include "cons.h"

static void cons_vdp1_clear(struct cons *, int32_t, int32_t, int32_t, int32_t);
static void cons_vdp1_reset(struct cons *);
static void cons_vdp1_scroll(struct cons *);
static void cons_vdp1_write(struct cons *, int, uint8_t, uint8_t);

void
cons_vdp1_init(struct cons *cons)
{

        cons->clear = cons_vdp1_clear;
        cons->reset = cons_vdp1_reset;
        cons->scroll = cons_vdp1_scroll;
        cons->write = cons_vdp1_write;

        cons_reset(cons);
}

static void
cons_vdp2_clear(struct cons *cons, int32_t col_start, int32_t col_end,
    int32_t row_start, int32_t row_end)
{
}

static void
cons_vdp1_reset(struct cons *cons)
{
}

static void
cons_vdp2_scroll(struct cons *cons __attribute__ ((unused)))
{
}

static void
cons_vdp1_write(struct cons *cons, int c, uint8_t fg, uint8_t bg)
{
}
