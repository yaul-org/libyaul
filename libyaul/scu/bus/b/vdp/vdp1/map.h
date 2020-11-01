/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _VDP1_MAP_H_
#define _VDP1_MAP_H_

#include <sys/cdefs.h>

#include <stdint.h>

/* VDP1 */
#define TVMR            0x0000UL
#define FBCR            0x0002UL
#define PTMR            0x0004UL
#define EWDR            0x0006UL
#define EWLR            0x0008UL
#define EWRR            0x000AUL
#define ENDR            0x000CUL
#define EDSR            0x0010UL
#define LOPR            0x0012UL
#define COPR            0x0014UL
#define MODR            0x0016UL

typedef union vdp1_registers {
        uint16_t buffer[11];

        struct {
                uint16_t tvmr;
                uint16_t fbcr;
                uint16_t ptmr;
                uint16_t ewdr;
                uint16_t ewlr;
                uint16_t ewrr;
                uint16_t endr;
                unsigned int :16;
                unsigned int :16;
                unsigned int :16;
                unsigned int :16;
        };
} __packed vdp1_registers_t;

#endif /* !_VDP1_MAP_H_ */
