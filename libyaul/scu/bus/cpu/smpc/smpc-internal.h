/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _SMPC_INTERNAL_H_
#define _SMPC_INTERNAL_H_

#include <scu-internal.h>

/* Specific macros */
#define IREG(x)         (0x20100001 + ((x) << 1))
#define OREG(x)         (0x20100021 + ((x) << 1))
#define SMPC(x)         (0x20100000 + (x))

#define SMPC_IREGS      7
#define SMPC_OREGS      31

#define SMPC_SMC_MSHON          0x00
#define SMPC_SMC_SSHON          0x02
#define SMPC_SMC_SSHOFF         0x03
#define SMPC_SMC_SNDON          0x06
#define SMPC_SMC_SNDOFF         0x07
#define SMPC_SMC_CDON           0x08
#define SMPC_SMC_CDOFF          0x09
#define SMPC_SMC_SYSRES         0x0D
#define SMPC_SMC_CKCHG352       0x0E
#define SMPC_SMC_CKCHG320       0x0F
#define SMPC_SMC_NMIREQ         0x18
#define SMPC_SMC_RESENAB        0x19
#define SMPC_SMC_RESDISA        0x1A
#define SMPC_SMC_INTBACK        0x10
#define SMPC_SMC_SETSMEM        0x17
#define SMPC_RTC_SETTIME        0x16

#define OREG_SET(x, y)  (oreg_buf[(x)] = (y))
#define OREG_GET(x)     (oreg_buf[(x)])
#define OREG_OFFSET(x)  (&oreg_buf[(x)])

/* 1st data byte */
#define PC_GET_MULTITAP_ID(x)   ((OREG_GET((x)) >> 4) & 0x0F)
#define PC_GET_NUM_CONNECTIONS(x) (OREG_GET((x)) & 0x0F)
/* 2nd data byte. */
#define PC_GET_ID(x)            (OREG_GET((x)) & 0xFF)
#define PC_GET_TYPE(x)          ((OREG_GET((x)) >> 4) & 0x0F)
#define PC_GET_SIZE(x)          (OREG_GET((x)) & 0x0F)
#define PC_GET_EXT_SIZE(x)      (OREG_GET((x) + 1) & 0xFF)
/* 3rd (or 4th) data byte */
#define PC_GET_DATA(x)          (OREG_OFFSET((x)))

#define SMPC_CMD_ISSUE_TYPE_A   0
#define SMPC_CMD_ISSUE_TYPE_B   1
#define SMPC_CMD_ISSUE_TYPE_C   2
#define SMPC_CMD_ISSUE_TYPE_D   3

/* SMPC */
#define COMREG          0x01F
#define SR              0x061
#define SF              0x063
#define PDR1            0x075
#define PDR2            0x077
#define DDR1            0x079
#define DDR2            0x07B
#define IOSEL1          0x07D
#define IOSEL2          0x07D
#define EXLE1           0x07F
#define EXLE2           0x07F

struct smpc_peripheral_port;

uint8_t smpc_smc_call(uint8_t, uint8_t, uint8_t *);

extern uint8_t offset;
extern uint8_t oreg_buf[];

extern struct smpc_peripheral_port smpc_peripheral_port1;
extern struct smpc_peripheral_port smpc_peripheral_port2;

#endif /* !_SMPC_INTERNAL_H_ */
