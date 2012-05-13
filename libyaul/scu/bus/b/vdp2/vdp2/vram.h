/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#ifndef _VRAM_H_
#define _VRAM_H_

#include <inttypes.h>
#include <stdbool.h>

#define VRAM_BANK_4MBIT(x, y)   (0x25E00000 + (((x) & 0x3) << 17) + (y))
#define VRAM_BANK_8MBIT(x, y)   (0x25E00000 + (((x) & 0x3) << 18) + (y))

struct vram_ctl {
#define VRAM_CTL_SIZE_4MBIT     0x0000
#define VRAM_CTL_SIZE_8MBIT     0x8000
        uint8_t vram_size;      /* VRAM size */

#define VRAM_CTL_MODE_NO_PART_BANK_A 0x0000
#define VRAM_CTL_MODE_NO_PART_BANK_B 0x0000
#define VRAM_CTL_MODE_PART_BANK_A 0x0100 /* Partition VRAM-A into two banks */
#define VRAM_CTL_MODE_PART_BANK_B 0x0200 /* Partition VRAM-B into two banks */
        uint16_t vram_mode;    /* VRAM mode bank partitions */

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
        } vram_cyc;
};

extern void vdp2_vram_control_set(struct vram_ctl *);

#endif /* !_VRAM_H_ */
