/*
 * Copyright (c) 2013-2014
 * See LICENSE for details.
 *
 * Mattias Jansson
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <stdio.h>

#include "int16.h"

int16_t
int16_vec2_dot(const int16_vec2_t *v0, const int16_vec2_t *v1)
{
        return (v0->x * v1->x) + (v0->y * v1->y);
}

void
int16_vec2_str(const int16_vec2_t *v0, char *buf)
{
        (void)sprintf(buf, "(%i,%i)", v0->x, v0->y);
}
