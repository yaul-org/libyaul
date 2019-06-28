/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _VDP2_VRAM_H_
#define _VDP2_VRAM_H_

#include <sys/cdefs.h>

#include <stdint.h>
#include <stdbool.h>

__BEGIN_DECLS

/*-
 *           4 Mbit
 * 0x000000 +----------+
 *          |    A0    |
 * 0x020000 +----------+
 *          |    A1    |
 * 0x040000 +----------+
 *          |    B0    |
 * 0x060000 +----------+
 *          |    B1    |
 * 0x080000 +----------+
 */

#define VDP2_VRAM_ADDR(x, y)    (0x25E00000 + ((x) << 17) + (y))

#define VDP2_VRAM_BANK(x)       (((x) >> 17) & 0x0007)

#define VDP2_VRAM_SIZE          (0x00080000)

/* 4 Mbit 2-split VRAM bank size */
#define VDP2_VRAM_BSIZE_2       (VDP2_VRAM_SIZE / 2)

/* 4 Mbit 4-split VRAM bank size */
#define VDP2_VRAM_BSIZE_4       (VDP2_VRAM_SIZE / 4)

struct vdp2_vram_ctl {
#define VDP2_VRAM_CTL_COEFFICIENT_TABLE_VRAM    0x0000 /* Store coefficient table in VRAM */
#define VDP2_VRAM_CTL_COEFFICIENT_TABLE_CRAM    0x0001 /* Store coefficient table in CRAM */
        uint8_t coefficient_table;

#define VDP2_VRAM_CTL_MODE_NO_PART_BANK_A       0x0000
#define VDP2_VRAM_CTL_MODE_NO_PART_BANK_B       0x0000
#define VDP2_VRAM_CTL_MODE_PART_BANK_A          0x0001 /* Partition VRAM-A into two banks */
#define VDP2_VRAM_CTL_MODE_PART_BANK_B          0x0002 /* Partition VRAM-B into two banks */
#define VDP2_VRAM_CTL_MODE_PART_BANK_BOTH       0x0003
        uint8_t vram_mode; /* VRAM mode bank partitions */
} __aligned(4);

#define VDP2_VRAM_CYCP_PNDR_NBG0        0x0 /* NBG0 pattern name data read */
#define VDP2_VRAM_CYCP_PNDR_NBG1        0x1 /* NBG1 pattern name data read */
#define VDP2_VRAM_CYCP_PNDR_NBG2        0x2 /* NBG2 pattern name data read */
#define VDP2_VRAM_CYCP_PNDR_NBG3        0x3 /* NBG3 pattern name data read */
#define VDP2_VRAM_CYCP_CHPNDR_NBG0      0x4 /* NBG0 character pattern name data read */
#define VDP2_VRAM_CYCP_CHPNDR_NBG1      0x5 /* NBG1 character pattern name data read */
#define VDP2_VRAM_CYCP_CHPNDR_NBG2      0x6 /* NBG2 character pattern name data read */
#define VDP2_VRAM_CYCP_CHPNDR_NBG3      0x7 /* NBG3 character pattern name data read */
#define VDP2_VRAM_CYCP_VCSTDR_NBG0      0xC /* NBG0 vertical cell scroll table data read */
#define VDP2_VRAM_CYCP_VCSTDR_NBG1      0xD /* NBG0 vertical cell scroll table data read */
#define VDP2_VRAM_CYCP_CPU_RW           0xE /* CPU read/write */
#define VDP2_VRAM_CYCP_NO_ACCESS        0xF /* No access */

/* Pattern name data read */
#define VDP2_VRAM_CYCP_PNDR(n)          ((n) & 0x03)

/* Character pattern name data read */
#define VDP2_VRAM_CYCP_CHPNDR(n)        (((n) & 0x03) + 0x04)

/* Vertical cell scroll table data read */
#define VDP2_VRAM_CYCP_VCSTDR(n)        (((n) & 0x01) + 0x0C)

struct vdp2_vram_cycp_bank {
        /*
         * General guideline for number of accesses required
         *
         *   1 - Pattern name data (1-word or 2-words)
         *   1 - 16-color character pattern or bitmapped data
         *   2 - 256-color character pattern or bitmapped data
         *   4 - 2048-color character pattern or bitmapped data
         *   4 - 32K-color character pattern or bitmapped data
         *   8 - 16M-color character pattern or bitmapped data
         *   1 - Vertical cell scroll table data
         */

        union {
                uint32_t raw;

                struct {
                        unsigned int t0:4; /* Timing T0 */
                        unsigned int t1:4; /* Timing T1 */
                        unsigned int t2:4; /* Timing T2 */
                        unsigned int t3:4; /* Timing T3 */
                        unsigned int t4:4; /* Timing T4 */
                        unsigned int t5:4; /* Timing T5 */
                        unsigned int t6:4; /* Timing T6 */
                        unsigned int t7:4; /* Timing T7 */
                } __packed;
        };
} __packed;

struct vdp2_vram_cycp {
        struct vdp2_vram_cycp_bank pt[4];
} __packed;

extern void vdp2_vram_control_set(const struct vdp2_vram_ctl *);

extern void vdp2_vram_cycp_set(const struct vdp2_vram_cycp *);
extern void vdp2_vram_cycp_clear(void);

extern struct vdp2_vram_cycp_bank vdp2_vram_cycp_bank_get(uint8_t);
extern void vdp2_vram_cycp_bank_set(uint8_t, const struct vdp2_vram_cycp_bank *);
extern void vdp2_vram_cycp_bank_clear(uint8_t);

__END_DECLS

#endif /* !_VDP2_VRAM_H_ */
