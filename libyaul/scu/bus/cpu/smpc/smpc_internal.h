/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#ifndef _SMPC_INTERNAL_H_
#define _SMPC_INTERNAL_H_

#include <inttypes.h>

#define MEM_POKE(x, y)  (*(volatile uint8_t *)(x) = (y))
#define MEM_READ(x)     (*(volatile uint8_t *)(x))

/* Macros specific for processor. */
#define IREG(x)         (0x20100001 + ((x) << 1))
#define OREG(x)         (0x20100021 + ((x) << 1))
#define SMPC(x)         (0x20100000 + (x))

enum smpc_regs_type {
        COMREG = 0x01f,
        SR = 0x061,
        SF = 0x063,
        PDR1 = 0x075,
        PDR2 = 0x077,
        DDR1 = 0x079,
        DDR2 = 0x07b,
        IOSEL1 = 0x07d,
        IOSEL2 = 0x07d,
        EXLE1 = 0x07f,
        EXLE2 = 0x07f
};

static inline void
smpc_cmd_call(uint8_t cmd)
{
        /* Wait until the command has finished executing. */
        for (; (MEM_READ(SMPC(SF)) & 0x01) == 0x01; );

        /* Execute the command. */
        MEM_POKE(SMPC(SF), 0x01);
        MEM_POKE(SMPC(COMREG), cmd);
}

#endif /* !_SMPC_INTERNAL_H_ */
