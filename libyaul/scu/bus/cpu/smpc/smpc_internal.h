/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#ifndef _SMPC_INTERNAL_H_
#define _SMPC_INTERNAL_H_

#include <stddef.h>
#include <inttypes.h>

#define MEM_POKE(x, y)  (*(volatile uint8_t *)(x) = (y))
#define MEM_READ(x)     (*(volatile uint8_t *)(x))

/* Macros specific for processor. */
#define IREG(x)         (0x20100001 + ((x) << 1))
#define OREG(x)         (0x20100021 + ((x) << 1))
#define SMPC(x)         (0x20100000 + (x))

#define SMPC_IREGS              7
#define SMPC_OREGS              31

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

#define OREG_SET(x, y)                  (oreg_buf[x] = (y))
#define OREG_GET(x)                     (oreg_buf[x])
#define OREG_OFFSET(x)                  (&oreg_buf[x])

/* 1st data byte */
#define PC_GET_MULTITAP_ID(x)           ((OREG_GET((x)) >> 4) & 0x0F)
#define PC_GET_NUM_CONNECTIONS(x)       (OREG_GET((x)) & 0x0F)
/* 2nd data byte. */
#define PC_GET_SIZE(x)                  (OREG_GET((x)) & 0x0F)
#define PC_GET_EXT_SIZE(x)              (OREG_GET((x) + 1) & 0xFF)
#define PC_GET_TYPE(x)                  ((OREG_GET((x)) >> 4) & 0x0F)
/* 3rd (or 4th) data byte */
#define PC_GET_DATA(x)                  (OREG_OFFSET((x)))

enum cmd_type {
        SMPC_CMD_ISSUE_TYPE_A,
        SMPC_CMD_ISSUE_TYPE_B,
        SMPC_CMD_ISSUE_TYPE_C,
        SMPC_CMD_ISSUE_TYPE_D
};

enum smpc_regs_type {
        COMREG = 0x01F,
        SR = 0x061,
        SF = 0x063,
        PDR1 = 0x075,
        PDR2 = 0x077,
        DDR1 = 0x079,
        DDR2 = 0x07b,
        IOSEL1 = 0x07D,
        IOSEL2 = 0x07D,
        EXLE1 = 0x07F,
        EXLE2 = 0x07F
};


static inline uint8_t
smpc_cmd_call(uint8_t cmd, enum cmd_type cmd_type, uint8_t *cmd_parameters)
{
        /* Wait until the command has finished executing. */
        for (; ((MEM_READ(SMPC(SF)) & 0x01) == 0x01); );

        /* Set the busy flag */
        MEM_POKE(SMPC(SF), 0x01);

        switch (cmd_type) {
        case SMPC_CMD_ISSUE_TYPE_A:
                /* Write command code */
                MEM_POKE(SMPC(COMREG), cmd);

                /* Wait for exception handling */
                break;
        case SMPC_CMD_ISSUE_TYPE_B:
                /* Write command code. */
                MEM_POKE(SMPC(COMREG), cmd);
                break;
        case SMPC_CMD_ISSUE_TYPE_C:
        case SMPC_CMD_ISSUE_TYPE_D:
                if (cmd_parameters == NULL) {
                        cmd_parameters[0] = 0x00;
                        cmd_parameters[1] = 0x00;
                        cmd_parameters[2] = 0xF0;
                }

                /* Write command parameters. */
                MEM_POKE(IREG(0), cmd_parameters[0]);
                MEM_POKE(IREG(1), cmd_parameters[1]);
                MEM_POKE(IREG(2), cmd_parameters[2]);

                /* Write command code. */
                MEM_POKE(SMPC(COMREG), cmd);
                break;
        }

        /* Wait until the command has finished executing. */
        for (; ((MEM_READ(SMPC(SF)) & 0x01) == 0x01); );

        return 0;
}

extern uint8_t offset;
extern uint8_t oreg_buf[];

#endif /* !_SMPC_INTERNAL_H_ */
