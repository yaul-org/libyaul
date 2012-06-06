/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <inttypes.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

#include "cons.h"

typedef struct {
} cons_vdp1_t;

static cons_vdp1_t *cons_vdp1_new(void);
static void cons_vdp1_reset(struct cons *);
static void cons_vdp1_write(struct cons *, int, uint8_t, uint8_t);

void
cons_vdp1_init(struct cons *cons)
{
        cons_vdp1_t *cons_vdp1;

        cons_vdp1 = cons_vdp1_new();

        cons->driver = cons_vdp1;

        cons->write = cons_vdp1_write;
        cons->reset = cons_vdp1_reset;

        cons_reset(cons);
}

static cons_vdp1_t *
cons_vdp1_new(void)
{
        /* XXX Replace with TLSF */
        static cons_vdp1_t cons_vdp1;
        return &cons_vdp1;
}

static void
cons_vdp1_reset(struct cons *cons)
{
        cons_vdp1_t *cons_vdp1;

        cons_vdp1 = cons->driver;

        /* Reset */
}

static void
cons_vdp1_write(struct cons *cons, int c, uint8_t fg, uint8_t bg)
{
        cons_vdp1_t *cons_vdp1;

        cons_vdp1 = cons->driver;
}
