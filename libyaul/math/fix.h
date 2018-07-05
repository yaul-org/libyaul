/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _FIX_H_
#define _FIX_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Q0.3.8 format */
typedef uint16_t q0_3_8_t;

#define Q0_3_8(v)       ((q0_3_8_t)(((float)(v)) * 256.0f))
#define Q0_3_8_INT(v)   ((((q0_3_8_t)(v)) >> 8) & 0x07)
#define Q0_3_8_FRAC(v)  (((q0_3_8_t)(v)) & 0xFF)

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* !_FIX_H_ */
