/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _VDP1_VRAM_H_
#define _VDP1_VRAM_H_

#include <inttypes.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define VDP1_CMDT_COUNT_MAX     2048
#define VDP1_CMDT_MEMORY_SIZE   (VDP1_CMDT_COUNT_MAX *                         \
            sizeof(struct vdp1_cmdt)) /* In bytes */

#define VDP1_TEXURE_MEMORY_SIZE ((432) * (1 << 10)) /* In bytes */

#define VDP1_GST_COUNT_MAX      1024
#define VDP1_GST_MEMORY_SIZE    (VDP1_GST_COUNT_MAX *                          \
            sizeof(struct vdp1_cmdt_gst)) /* In bytes */

#define VDP1_CLUT_COUNT_MAX     256
#define VDP1_CLUT_MEMORY_SIZE   (VDP1_CLUT_COUNT_MAX *                         \
            (16 * sizeof(uint16_t))) /* In bytes */

#define CMD_TABLE(x, y)         (0x25C00000 + ((x) << 5) + (((y) << 1) & 0x1F))
#define CLUT(x, y)              (0x25C00000 + VDP1_CMDT_MEMORY_SIZE +          \
            ((x) << 5) + ((y) << 1))
#define GOURAUD(x, y)           (0x25C00000 + VDP1_CMDT_MEMORY_SIZE +          \
            VDP1_CLUT_MEMORY_SIZE + ((x) << 3) + ((y) << 1))
#define CHAR(x)                 (0x25C00000 + VDP1_CMDT_MEMORY_SIZE +          \
            VDP1_CLUT_MEMORY_SIZE + VDP1_GST_MEMORY_SIZE + ((x)))
#define VDP1_FRAME_BUFFER(x, y) (0x25C80000 + (((x) & 0x1) << 18) + (y))

#define VDP1_VRAM_SIZE          0x80000 /* In bytes */

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* !_VDP1_VRAM_H_ */
