/*
 * Copyright (c) 2013-2014
 * See LICENSE for details.
 *
 * Mattias Jansson
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <stdio.h>

#include "vector2.h"

void
int16_vector2_normalize(int16_vector2_t *v0 __unused__)
{
}

int16_t
int16_vector2_length(const int16_vector2_t *v0 __unused__)
{
        return 0;
}

void
int16_vector2_add(const int16_vector2_t *v0, const int16_vector2_t *v1,
    int16_vector2_t *result)
{
        result->x = v0->x + v1->x;
        result->y = v0->y + v1->y;
}

void
int16_vector2_sub(const int16_vector2_t *v0, const int16_vector2_t *v1,
    int16_vector2_t *result)
{
        result->x = v0->x - v1->x;
        result->y = v0->y - v1->y;
}

void
int16_vector2_scale(int8_t scalar, int16_vector2_t *result)
{
        result->x = scalar * result->x;
        result->y = scalar * result->y;
}

int16_t
int16_vector2_dot(const int16_vector2_t *v0, const int16_vector2_t *v1)
{
        return (v0->x * v1->x) + (v0->y * v1->y);
}

void
int16_vector2_str(const int16_vector2_t *v0, char *buf)
{
        (void)sprintf(buf, "(%i,%i)", v0->x, v0->y);
}
