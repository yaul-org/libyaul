/*
 * Copyright (c) 2013-2014
 * See LICENSE for details.
 *
 * Mattias Jansson
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <stdio.h>

#include "int32.h"

void
int32_vec2_str(const int32_vec2_t *v0, char *buffer)
{
        (void)sprintf(buffer, "(%i,%i)", v0->x, v0->y);
}
