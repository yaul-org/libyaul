/*
 * Copyright (c) 2013-2014
 * See LICENSE for details.
 *
 * Mattias Jansson
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <stdio.h>

#include "int16.h"

void
int16_vec2_str(const int16_vec2_t *v0, char *buf)
{
        (void)sprintf(buf, "(%i,%i)", v0->x, v0->y);
}
