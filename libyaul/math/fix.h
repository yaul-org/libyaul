/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_MATH_FIX_H_
#define _YAUL_MATH_FIX_H_

__BEGIN_DECLS

/* Q0.3.8 format */
typedef uint16_t q0_3_8_t;

#define Q0_3_8(v)       ((q0_3_8_t)(((float)(v)) * 256.0f))
#define Q0_3_8_INT(v)   ((((q0_3_8_t)(v)) >> 8) & 0x07)
#define Q0_3_8_FRAC(v)  (((q0_3_8_t)(v)) & 0xFF)

__END_DECLS

#endif /* !_YAUL_MATH_FIX_H_ */
