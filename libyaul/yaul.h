/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _LIBYAUL_H_
#define _LIBYAUL_H_

/* Check if HAVE_DEV_CARTRIDGE has a valid value */
/* USB flash cartridge */
/* Datel Action Replay cartridge */
#if (HAVE_DEV_CARTRIDGE < 0) || (HAVE_DEV_CARTRIDGE > 2)
#error "Invalid `HAVE_DEV_CARTRIDGE' value"
#endif

/* CPU-bus CPU */
#include <scu/bus/cpu/cpu.h>

/* CPU-bus SMPC */
#include <scu/bus/cpu/smpc/smpc.h>

/* SCU */
#include <scu/scu.h>

/* CS0 */
//#include <scu/bus/a/cs0/usb-cartridge/usb-cartridge.h>
#include <scu/bus/a/cs0/arp/arp.h>
#include <scu/bus/a/cs0/dram-cartridge/dram-cartridge.h>

/* CS2 */
#include <scu/bus/a/cs2/cd-block/cd-block.h>

/* B-bus VDP1 */
#include <scu/bus/b/vdp1/vdp1.h>

/* B-bus VDP2 */
#include <scu/bus/b/vdp2/vdp2.h>

/* B-bus SCSP */
#include <scu/bus/b/scsp/scsp.h>

/* Kernel */
#include <common/common.h>
#include <common/irq-mux.h>

#include <common/gdb/gdb.h>

#include <cons/cons.h>

#include <math.h>

#include <lib/memb.h>

#include <fs/iso9660/iso9660.h>
#include <fs/romdisk/romdisk.h>

#endif /* !_LIBYAUL_H_ */
