/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef GLOBALS_H
#define GLOBALS_H

#include <yaul.h>

#define FIXMATH_NO_OVERFLOW 1
#define FIXMATH_NO_ROUNDING 1
#include <fixmath.h>

#include <inttypes.h>

#define RGB888_TO_RGB555(r, g, b) (0x8000 | (((b) >> 3) << 10) |               \
    (((g) >> 3) << 5) | ((r) >> 3))

#define SCREEN_WIDTH    320
#define SCREEN_HEIGHT   224

extern struct cons cons;
extern struct smpc_peripheral_digital digital_pad;
extern uint32_t tick;

void sleep(fix16_t);

#endif /* !GLOBALS_H */
