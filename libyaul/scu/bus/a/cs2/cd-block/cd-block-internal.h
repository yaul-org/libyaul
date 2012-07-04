/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _CD_BLOCK_INTERNAL_H_
#define _CD_BLOCK_INTERNAL_H_

#include <scu-internal.h>

/* Specific macros */
#define CD_BLOCK(x)         (0x25890000 + (x))

struct cd_block_regs {
        uint16_t dtr;
        uint16_t hirq;
        uint16_t hirq_mask;
        uint16_t cr1;
        uint16_t cr2;
        uint16_t cr3;
        uint16_t cr4;
};

/* CD-block */
#define DTR             0x0000
#define HIRQ            0x0008
#define HIRQ_MASK       0x000C
#define CR1             0x0018
#define CR2             0x001C
#define CR3             0x0020
#define CR4             0x0024

int cd_block_cmd_execute(struct cd_block_regs *, struct cd_block_regs *);

#endif /* !_CD_BLOCK_INTERNAL_H_ */
