/*
 * Copyright (c) 2012
 * See LICENSE for details.
 *
 * Theo Beraku
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _CD_BLOCK_INTERNAL_H_
#define _CD_BLOCK_INTERNAL_H_

#include <scu-internal.h>

/* Specific macros */
#define CD_BLOCK(x)     (0x25890000 + (x))

/* Helpers */
#define FAD2LBA(x)      ((x) - 150)
#define LBA2FAD(x)      ((x) + 150)

#define CMOK            0x0001 /* Command dispatch possible */
#define DRDY            0x0002 /* Data transfer preparations complete */
#define CSCT            0x0004 /* Finished reading 1 sector */
#define BFUL            0x0008 /* CD buffer full */
#define PEND            0x0010 /* CD playback completed */
#define DCHG            0x0020 /* Disc change or tray open */
#define ESEL            0x0040 /* Selector settings processing complete */
#define EHST            0x0080 /* Host I/O processing complete */
#define ECPY            0x0100 /* Duplication/move processing complete */
#define EFLS            0x0200 /* File system processing complete */
#define SCDQ            0x0400 /* Subcode Q update completed */
#define MPED            0x0800 /* MPEG-related processing complete */
#define MPCM            0x1000 /* MPEG action uncertain */
#define MPST            0x2000 /* MPEG interrupt status report */

struct cd_block_regs {
        uint16_t dtr;
        uint16_t hirq;
        uint16_t hirq_mask;
        uint16_t cr1;
        uint16_t cr2;
        uint16_t cr3;
        uint16_t cr4;
};

struct cd_block_status {
#define CD_STATUS_BUSY          0x00
#define CD_STATUS_PAUSE         0x01
#define CD_STATUS_STANDBY       0x02
#define CD_STATUS_PLAY          0x03
#define CD_STATUS_SEEK          0x04
#define CD_STATUS_SCAN          0x05
#define CD_STATUS_OPEN          0x06
#define CD_STATUS_NO_DISC       0x07
#define CD_STATUS_RETRY         0x08
#define CD_STATUS_ERROR         0x09
#define CD_STATUS_FATAL         0x0A /* Fatal error (hard reset required) */
#define CD_STATUS_PERI          0x20 /* Periodic response if set, else command response */
#define CD_STATUS_TRANS         0x40 /* Data transfer request if set */
#define CD_STATUS_WAIT          0x80
#define CD_STATUS_REJECT        0xFF
        uint8_t cd_status;
        uint8_t flag;
        uint8_t repeat_count;
        uint8_t ctrl_addr;
        uint8_t track;
        uint8_t index;
        uint32_t fad;
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
