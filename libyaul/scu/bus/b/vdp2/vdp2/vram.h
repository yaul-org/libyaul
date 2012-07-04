/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _VRAM_H_
#define _VRAM_H_

#include <inttypes.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define VRAM_ADDR_4MBIT(x, y)   (0x25E00000 + (((x) & 0x03) << 17) + (y))
#define VRAM_ADDR_8MBIT(x, y)   (0x25E00000 + (((x) & 0x03) << 18) + (y))

struct vram_ctl {
#define VRAM_CTL_SIZE_4MBIT     0x0000
#define VRAM_CTL_SIZE_8MBIT     0x8000
        uint16_t vram_size;     /* VRAM size */

#define VRAM_CTL_MODE_NO_PART_BANK_A 0x0000
#define VRAM_CTL_MODE_NO_PART_BANK_B 0x0000
#define VRAM_CTL_MODE_PART_BANK_A 0x0100 /* Partition VRAM-A into two banks */
#define VRAM_CTL_MODE_PART_BANK_B 0x0200 /* Partition VRAM-B into two banks */
        uint16_t vram_mode;     /* VRAM mode bank partitions */

/*
 * Number of accesses required:
 * 1 - Pattern name data (1-word or 2-word)
 * 1 - 16-color character pattern or bitmapped data
 * 2 - 256-color character pattern or bitmapped data
 * 4 - 2048-color character pattern or bitmapped data
 * 4 - 32K-color character pattern or bitmapped data
 * 8 - 16M-color character pattern or bitmapped data
 * 1 - Vertical cell scroll table data
 */
#define VRAM_CTL_CYCP_PNDR_NBG0 0x0 /* NBG0 pattern name data read */
#define VRAM_CTL_CYCP_PNDR_NBG1 0x1 /* NBG1 pattern name data read */
#define VRAM_CTL_CYCP_PNDR_NBG2 0x2 /* NBG2 pattern name data read */
#define VRAM_CTL_CYCP_PNDR_NBG3 0x3 /* NBG3 pattern name data read */
#define VRAM_CTL_CYCP_CHPNDR_NBG0 0x4 /* NBG0 character pattern name data read */
#define VRAM_CTL_CYCP_CHPNDR_NBG1 0x5 /* NBG1 character pattern name data read */
#define VRAM_CTL_CYCP_CHPNDR_NBG2 0x6 /* NBG2 character pattern name data read */
#define VRAM_CTL_CYCP_CHPNDR_NBG3 0x7 /* NBG3 character pattern name data read */
#define VRAM_CTL_CYCP_VCSTDR_NBG0 0xC /* NBG0 vertical cell scroll table data read */
#define VRAM_CTL_CYCP_VCSTDR_NBG1 0xD /* NBG0 vertical cell scroll table data read */
#define VRAM_CTL_CYCP_CPU_RW 0xE /* CPU read/write */
#define VRAM_CTL_CYCP_NO_ACCESS 0xF /* No access */
        union {
                uint32_t pv[4]; /* VRAM cycle pattern value */
                struct {
                        unsigned int t0:4; /* Timing T0 */
                        unsigned int t1:4; /* Timing T1 */
                        unsigned int t2:4; /* Timing T2 */
                        unsigned int t3:4; /* Timing T3 */
                        unsigned int t4:4; /* Timing T4 */
                        unsigned int t5:4; /* Timing T5 */
                        unsigned int t6:4; /* Timing T6 */
                        unsigned int t7:4; /* Timing T7 */
                } __attribute__ ((packed)) pt[4];
        } vram_cycp;
};

extern struct vram_ctl *vdp2_vram_control_get(void);
extern void vdp2_vram_control_set(struct vram_ctl *);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* !_VRAM_H_ */
