/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include "smpc-internal.h"

static void wait(void);

uint8_t
smpc_smc_call(uint8_t cmd, uint8_t cmd_type, uint8_t *cmd_parameters)
{
        /* Wait until the command has finished executing */
        wait();

        /* Set the busy flag */
        MEMORY_WRITE(8, SMPC(SF), 0x01);

        switch (cmd_type) {
        case SMPC_CMD_ISSUE_TYPE_A:
                /* Write command code */
                MEMORY_WRITE(8, SMPC(COMREG), cmd);

                /* Wait for exception handling */
                break;
        case SMPC_CMD_ISSUE_TYPE_B:
                /* Write command code */
                MEMORY_WRITE(8, SMPC(COMREG), cmd);
                break;
        case SMPC_CMD_ISSUE_TYPE_C:
        case SMPC_CMD_ISSUE_TYPE_D:
                if (cmd_parameters == NULL) {
                        cmd_parameters[0] = 0x00;
                        cmd_parameters[1] = 0x00;
                        cmd_parameters[2] = 0xF0;
                }

                /* Write command parameters */
                MEMORY_WRITE(8, IREG(0), cmd_parameters[0]);
                MEMORY_WRITE(8, IREG(1), cmd_parameters[1]);
                MEMORY_WRITE(8, IREG(2), cmd_parameters[2]);

                /* Write command code */
                MEMORY_WRITE(8, SMPC(COMREG), cmd);
                break;
        }

        /* Wait until the command has finished executing */
        wait();

        return 0;
}

static void
wait(void)
{
        /* Wait until the command has finished executing */
        for (; ((MEMORY_READ(8, SMPC(SF)) & 0x01) == 0x01); );
}
