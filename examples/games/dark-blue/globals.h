/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef GLOBALS_H
#define GLOBALS_H

#include <yaul.h>

#include <inttypes.h>

extern struct smpc_peripheral_digital digital_pad;
extern uint32_t tick;
extern uint32_t start_scanline;
extern uint32_t end_scanline;
extern char text[];

#endif /* !GLOBALS_H */
