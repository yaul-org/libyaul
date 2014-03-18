/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef GLOBALS_H
#define GLOBALS_H

#include <smpc.h>
#include <smpc/peripheral.h>

extern struct smpc_peripheral_digital g_digital;

extern volatile uint32_t g_frame_counter;

void sleep(uint32_t);

#endif /* !GLOBALS_H */
