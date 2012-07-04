/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _DRAM_CARTRIDGE_H__
#define _DRAM_CARTRIDGE_H__

#include <inttypes.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define DRAM_CARTRIDGE_ID_1MIB  0x5A
#define DRAM_CARTRIDGE_ID_4MIB  0x5C

#define DRAM_CARTRIDGE_BANKS    0x04

extern size_t dram_cartridge_size(void);
extern uint8_t dram_cartridge_id(void);
extern void *dram_cartridge_area(void);
extern void dram_cartridge_init(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* !_DRAM_CARTRIDGE_H_ */
