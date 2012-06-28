/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#ifndef _CMDT_H_
#define _CMDT_H_

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct cmdt_cmd {
        uint16_t ctrl;
        uint16_t link;
        uint16_t pmod;
        uint16_t colr;
        uint16_t srca;
        uint16_t size;
        int16_t xa;
        int16_t ya;
        int16_t xb;
        int16_t yb;
        int16_t xc;
        int16_t yc;
        int16_t xd;
        int16_t yd;
        uint16_t grda;
} __attribute__ ((packed, aligned (32)));

struct cmdt_grd {
        uint16_t entry[4];
} __attribute__ ((packed, aligned (8)));

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* !_CMDT_H_ */
