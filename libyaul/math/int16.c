/*
 * Copyright (c) 2013-2014
 * See LICENSE for details.
 *
 * Mattias Jansson
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <stdio.h>

#include "int16.h"

size_t
int16_vec2_str(const int16_vec2_t *v0, char *buffer)
{
        return sprintf(buffer, "(%i,%i)", v0->x, v0->y);
}
