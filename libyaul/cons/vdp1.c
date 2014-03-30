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

static void cons_vdp1_scroll(struct cons *);
static void cons_vdp1_write(struct cons *);

void
cons_vdp1_init(struct cons *cons)
{

        cons->scroll = cons_vdp1_scroll;
        cons->write = cons_vdp1_write;
}

static void
cons_vdp2_scroll(struct cons *cons __attribute__ ((unused)))
{
}

static void
cons_vdp1_scroll(struct cons *cons __attribute__ ((unused)))
{
}

static void
cons_vdp1_write(struct cons *cons)
{
}
