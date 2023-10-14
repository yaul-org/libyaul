/*
 * Copyright (c) Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <stdio.h>

#include <gamemath/int32.h>

size_t
int32_vec2_str(const int32_vec2_t *v0, char *buffer)
{
    return sprintf(buffer, "(%i,%i)", v0->x, v0->y);
}
