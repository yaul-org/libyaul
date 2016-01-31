/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _SMPC_INTERNAL_H_
#define _SMPC_INTERNAL_H_

#include <scu-internal.h>

#include <smpc/map.h>

#define SMPC_IREGS              7
#define SMPC_OREGS              32

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

#define SMPC_CMD_ISSUE_TYPE_A   0
#define SMPC_CMD_ISSUE_TYPE_B   1
#define SMPC_CMD_ISSUE_TYPE_C   2
#define SMPC_CMD_ISSUE_TYPE_D   3

struct smpc_peripheral_port;

uint8_t smpc_smc_call(uint8_t, uint8_t, uint8_t *);

extern struct smpc_peripheral_port smpc_peripheral_port_1;
extern struct smpc_peripheral_port smpc_peripheral_port_2;

#endif /* !_SMPC_INTERNAL_H_ */
