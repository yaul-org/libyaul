/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _LIBYAUL_H_
#define _LIBYAUL_H_

/* CPU */
#include <cpu.h>

/* SCU */
#include <scu.h>

/* CS0 */
#include <arp.h>
#include <dram-cartridge.h>
#include <usb-cartridge.h>

/* CS2 */
#include <cd-block.h>

/* VDP1 */
#include <vdp1.h>

/* VDP2 */
#include <vdp2.h>

/* SMPC */
#include <smpc.h>

/* Kernel */
#include <common.h>
#include <irq-mux.h>
#include <cons.h>

#include <fs/iso9660/iso9660.h>
#include <fs/romdisk/romdisk.h>

#include <gdb.h>

#endif /* !_LIBYAUL_H_ */
