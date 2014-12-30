/*
 * Copyright (c) 2013-2014
 * See LICENSE for details.
 *
 * Mattias Jansson
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <stdio.h>

#include "types.h"

fix16_matrix4_t *
fix16_matrix_zero(void)
{
        return NULL;
}

fix16_matrix4_t *
fix16_matrix_identity(void)
{
        return NULL;
}

void
fix16_matrix_multiply(const fix16_matrix4_t *m0 __unused__,
    const fix16_matrix4_t *m1 __unused__,
    fix16_matrix4_t *result __unused__)
{
}

void
fix16_matrix4_transpose(const fix16_matrix4_t *m0 __unused__,
    fix16_matrix4_t *result __unused__)
{
}
