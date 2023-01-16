#ifndef MIC3D_MATRIX_H
#define MIC3D_MATRIX_H

#include <fix16.h>

void __matrix_init(void);

fix16_mat43_t *__matrix_view_get(void);

#endif /* MIC3D_MATRIX_H */
