/*
 * Copyright (c) 2022-2023 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _MIC3D_MATRIX_H_
#define _MIC3D_MATRIX_H_

#include <gamemath/fix16.h>

typedef struct mstack {
    fix16_mat43_t *pool_matrix;
    fix16_mat43_t *top_matrix;
    fix16_mat43_t *bottom_matrix;
    uint32_t stack_count;
} mstack_t;

void __matrix_init(void);

fix16_mat43_t *__matrix_view_get(void);

#endif /* _MIC3D_MATRIX_H_ */
