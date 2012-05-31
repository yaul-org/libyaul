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

#include <cons.h>

void
cons_write(struct cons *cons, const char *s)
{
}

void
cons_reset(struct cons *cons)
{

        cons->cursor.col = 0;
        cons->cursor.row = 0;
}
