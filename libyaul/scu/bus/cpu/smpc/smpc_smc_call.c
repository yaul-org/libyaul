/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>
#include <stdbool.h>

#include "smpc-internal.h"

#define WAIT() do {                                                            \
        for (; ((MEMORY_READ(8, SMPC(SF)) & 0x01) == 0x01); );                 \
} while (false)

uint8_t
smpc_smc_call(uint8_t cmd, uint8_t cmd_type, uint8_t *cmd_parameters)
{
        /* Wait until the command has finished executing */
        WAIT();

        /* Set the busy flag */
        MEMORY_WRITE(8, SMPC(SF), 0x01);

        switch (cmd_type) {
        case SMPC_CMD_ISSUE_TYPE_A:
                /* Write command code */
                MEMORY_WRITE(8, SMPC(COMREG), cmd);

                /* Wait until the command has finished executing */
                WAIT();

                /* Wait for exception handling */

                return 0;
        case SMPC_CMD_ISSUE_TYPE_B:
                /* Write command code */
                MEMORY_WRITE(8, SMPC(COMREG), cmd);

                /* Wait until the command has finished executing */
                WAIT();

                return 0;
        case SMPC_CMD_ISSUE_TYPE_C:
        case SMPC_CMD_ISSUE_TYPE_D:
                assert(cmd_parameters != NULL);

                /* Write command parameters */
                MEMORY_WRITE(8, IREG(0), cmd_parameters[0]);
                MEMORY_WRITE(8, IREG(1), cmd_parameters[1]);
                MEMORY_WRITE(8, IREG(2), 0xF0);

                /* Write command code */
                MEMORY_WRITE(8, SMPC(COMREG), cmd);

                return 0;
        }
}
