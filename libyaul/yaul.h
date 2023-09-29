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
#include <bup.h>
#include <ip.h>

#include <cpu.h>
#include <smpc.h>

#include <scu.h>

/* CS0 */
#include <usb-cart.h>
#include <arp.h>
#include <flash.h>
#include <dram-cart.h>

/* CS2 */
#include <cd-block.h>

#include <vdp.h>
#include <scsp.h>

/* Kernel */
#include <dbgio.h>

#include <gamemath.h>

#include <mm/arena.h>
#include <mm/memb.h>
#include <mm/pagep.h>
#include <mm/mm_stats.h>

#if defined(MALLOC_IMPL_TLSF)
#include <mm/tlsf.h>
#endif /* MALLOC_IMPL_TLSF */

#include <sys/init.h>
#include <sys/dma-queue.h>

#include <fs/cd/cdfs.h>

#endif /* !_YAUL_H_ */
