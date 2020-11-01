#include "sega3d.h"

#include "sega3d-internal.h"

/*
 * Increments the matrix stack pointer and copies the current matrix to that
 * position. Up to 20 levels of matrices can be nested, but an error is returned
 * if this number is exceeded.
 */

/* Increments the matrix stack pointer and sets the unit matrix at that
 * location. An error is returned when excessive nesting occurs. */

/* Increments the matrix stack pointer. Up to 20 matrices can be nested, but an
 * error is returned if this number is exceeded. */

void
sega3d_matrix_push(/* matrix_type_t matrix_type */)
{
}

void
sega3d_matrix_pop(/* matrix_type_t matrix_type */)
{
}

/* void slLoadMatrix(MATRIX *mtptr) */
/* Copies a specified matrix to the current matrix. */
void
sega3d_matrix_load(void)
{
}

/* Copies the penultimate matrix to the current matrix. When the matrix is not
 * nested, an error is returned. */
void
sega3d_matrix_dup(void)
{
}

void
sega3d_matrix_translate()
{
}

/* void slUnitMatrix(MATRIX *mtptr) */
/* void slUnitAngle(MATRIX *mtptr) */
/* void slUnitTranslate(MATRIX *mtptr) */

/* void slTranslate(FIXED tx, FIXED ty, FIXED tz) */
