/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _LIBYAUL_H_
#define _LIBYAUL_H_

/* CPU-bus CPU */
#include <yaul/scu/bus/cpu/cpu.h>

/* CPU-bus SMPC */
#include <yaul/scu/bus/cpu/smpc/smpc.h>

/* SCU */
#include <yaul/scu/scu.h>

/* CS0 */
#include <yaul/scu/bus/a/cs0/arp/arp.h>
#include <yaul/scu/bus/a/cs0/dram-cartridge/dram-cartridge.h>
#include <yaul/scu/bus/a/cs0/usb-cartridge/usb-cartridge.h>

/* CS2 */
#include <yaul/scu/bus/a/cs2/cd-block/cd-block.h>

/* B-bus VDP1 */
#include <yaul/scu/bus/b/vdp1/vdp1.h>

/* B-bus VDP2 */
#include <yaul/scu/bus/b/vdp2/vdp2.h>

/* B-bus SCSP */
#include <yaul/scu/bus/b/scsp/scsp.h>

/* Kernel */
#include <yaul/common/common.h>
#include <yaul/common/irq-mux.h>

#include <yaul/common/gdb/gdb.h>

#include <yaul/cons/cons.h>

#include <yaul/lib/memb.h>

#include <yaul/fs/iso9660/iso9660.h>
#include <yaul/fs/romdisk/romdisk.h>

#endif /* !_LIBYAUL_H_ */
