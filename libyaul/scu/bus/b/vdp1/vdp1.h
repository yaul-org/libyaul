/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _VDP1_H_
#define _VDP1_H_

#include <vdp1/cmdt.h>
#include <vdp1/fbcr.h>
#include <vdp1/map.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define VDP1_CMDT_COUNT_MAX             544
#define VDP1_CMDT_MEMORY_SIZE           (VDP1_CMDT_COUNT_MAX * sizeof(struct vdp1_cmdt)) /* In bytes */

#define VDP1_TEXURE_MEMORY_SIZE         ((486) * (1 << 10)) /* In bytes */

#define VDP1_GST_COUNT_MAX              128
#define VDP1_GST_MEMORY_SIZE            (VDP1_GST_COUNT_MAX * sizeof(struct vdp1_cmdt_gst)) /* In bytes */

#define VDP1_CLUT_COUNT_MAX             256
#define VDP1_CLUT_MEMORY_SIZE           (VDP1_CLUT_COUNT_MAX * (16 * sizeof(uint16_t))) /* In bytes */

extern void vdp1_init(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
