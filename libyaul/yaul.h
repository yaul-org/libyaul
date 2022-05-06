/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

/// @defgroup MEMORY_MAP Memory map
/// @defgroup MEMORY_MAP_MACROS Accessors
/// @ingroup MEMORY_MAP
/// @defgroup MEMORY_MAP_AREAS Offset Accessors
/// @ingroup MEMORY_MAP

#ifndef _YAUL_H_
#define _YAUL_H_

#include <bios.h>

/* CPU-bus CPU */
#include <scu/bus/cpu/cpu.h>

/* CPU-bus SMPC */
#include <scu/bus/cpu/smpc/smpc.h>

/* SCU */
#include <scu/scu.h>

/* CS0 */
#include <usb-cart.h>
#include <arp.h>
#include <flash.h>

#include <scu/bus/a/cs0/dram-cart/dram-cart.h>

/* CS2 */
#include <scu/bus/a/cs2/cd-block/cd-block.h>

/* B-bus VDP1 and VDP2 */
#include <scu/bus/b/vdp/vdp.h>

/* B-bus SCSP */
#include <scu/bus/b/scsp/scsp.h>

/* Kernel */
#include <ip/ip.h>

#include <gdb/gdb.h>

#include <dbgio/dbgio.h>

#include <math.h>

#include <mm/memb.h>
#include <mm/mm_stats.h>

#if defined(MALLOC_IMPL_TLSF)
#include <mm/tlsf.h>
#endif /* MALLOC_IMPL_TLSF */

#include <sys/init.h>
#include <sys/dma-queue.h>

#include <ssload.h>

#include <fs/cd/cdfs.h>

#endif /* !_YAUL_H_ */
