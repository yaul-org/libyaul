/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _DRAM_CART_H_
#define _DRAM_CART_H_

#include <sys/cdefs.h>

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

__BEGIN_DECLS

#define DRAM_CART_ID_1MIB  0x5A
#define DRAM_CART_ID_4MIB  0x5C

#define DRAM_CART_BANKS    0x04

extern void dram_cart_init(void);
extern void *dram_cart_area_get(void);
extern uint8_t dram_cart_id_get(void);
extern size_t dram_cart_size_get(void);

__END_DECLS

#endif /* !_DRAM_CART_H_ */
