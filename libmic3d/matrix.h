#ifndef _MIC3D_MATRIX_H_
#define _MIC3D_MATRIX_H_

#include <fix16.h>

void __matrix_init(void);

fix16_mat43_t *__matrix_view_get(void);

#endif /* _MIC3D_MATRIX_H_ */
