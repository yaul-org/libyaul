/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_SMPC_SMC_H_
#define _YAUL_SMPC_SMC_H_

#include <sys/cdefs.h>

#include <stdint.h>

#include <smpc/map.h>

__BEGIN_DECLS

/// @defgroup SMPC_SMC SMPC SMC

/// @addtogroup SMPC_SMC
/// @{

/// @brief SMPC commands
typedef enum cpu_smpc_cmd {
        /// SMPC command to turn master CPU on.
        SMPC_SMC_MSHON    = 0x00,
        /// SMPC command to turn slave CPU on.
        SMPC_SMC_SSHON    = 0x02,
        /// SMPC command to turn slave CPU off.
        SMPC_SMC_SSHOFF   = 0x03,
        /// SMPC command to turn sound CPU on.
        SMPC_SMC_SNDON    = 0x06,
        /// SMPC command to turn sound CPU off.
        SMPC_SMC_SNDOFF   = 0x07,
        /// SMPC command to turn CD-block on.
        SMPC_SMC_CDON     = 0x08,
        /// SMPC command to turn CD-block off.
        SMPC_SMC_CDOFF    = 0x09,
        /// SMPC command to reset the system.
        SMPC_SMC_SYSRES   = 0x0D,
        /// SMPC command to change clock to 352-mode.
        SMPC_SMC_CKCHG352 = 0x0E,
        /// SMPC command to change clock to 320-mode.
        SMPC_SMC_CKCHG320 = 0x0F,
        /// SMPC command to send an NMI request to the master CPU.
        SMPC_SMC_NMIREQ   = 0x18,
        /// SMPC command to enable NMI generation.
        SMPC_SMC_RESENAB  = 0x19,
        /// SMPC command to disable NMI generation.
        SMPC_SMC_RESDISA  = 0x1A,
        /// SMPC command to acquire SMPC status and peripheral data.
        SMPC_SMC_INTBACK  = 0x10,
        /// SMPC command to set data to SMEM.
        SMPC_SMC_SETSMEM  = 0x17,
        /// SMPC command to set RTC time.
        SMPC_RTC_SETTIME  = 0x16
} cpu_smpc_cmd_t;

/// @brief Wait either to call an SMPC command, or wait until called SMPC
/// command has ended.
///
/// @details If @p enter is `false`, wait until the called SMPC command has
/// ended. If @p is `true`, then wait until the SMPC is ready for another
/// command to be called.
///
/// @note There is no real need to call use this function, as there are inlined
/// functions that handle the call to SMPC commands.
///
/// @details enter Wait to call or wait for SMPC command
///
/// @returns The value of the 31st `OREG` register.
static inline void __always_inline
smpc_smc_wait(bool enter)
{
        volatile uint8_t *reg_sf;
        reg_sf = (volatile uint8_t *)SMPC(SF);

        while ((*reg_sf & 0x01) == 0x01);

        if (enter) {
                *reg_sf = 0x01;
        }
}

/// @brief Call an SMPC command.
///
/// @note There is no real need to call use this function, as there are inlined
/// functions that handle the call to SMPC commands.
///
/// @param cmd The SMPC command
static inline uint8_t __always_inline
smpc_smc_call(cpu_smpc_cmd_t cmd)
{
        smpc_smc_wait(true);

        MEMORY_WRITE(8, SMPC(COMREG), cmd);

        smpc_smc_wait(false);

        return MEMORY_READ(8, OREG(31));
}

/// @brief Call SMPC command to turn CD-block off.
///
/// @warning Do not use this function.
static inline uint8_t __always_inline
smpc_smc_cdoff_call(void)
{
        return smpc_smc_call(SMPC_SMC_CDOFF);
}

/// @brief SMPC command to turn CD-block on.
///
/// @warning Do not use this function.
static inline uint8_t __always_inline
smpc_smc_cdon_call(void)
{
        return smpc_smc_call(SMPC_SMC_CDON);
}

/// @brief Call SMPC command to change clock to 320-mode.
///
/// @warning Do not use this function.
static inline uint8_t __always_inline
smpc_smc_ckchg320_call(void)
{
        return smpc_smc_call(SMPC_SMC_CKCHG320);
}

/// @brief Call SMPC command to change clock to 352-mode.
///
/// @warning Do not use this function.
static inline uint8_t __always_inline
smpc_smc_ckchg352_call(void)
{
        return smpc_smc_call(SMPC_SMC_CKCHG352);
}

/// @brief Call SMPC command to acquire SMPC status and peripheral data.
///
/// @details This function does not wait until the SMPC INTBACK command ends.
/// This is purposely done for asynchronous reasons.
///
/// @warning Do not use this function, as Yaul uses this function to SMPC status
/// and fetch peripheral data.
static inline uint8_t __always_inline
smpc_smc_intback_call(uint8_t ireg0, uint8_t ireg1)
{
        /* Fetch peripheral data "SMPC" status */
        smpc_smc_wait(true);

        MEMORY_WRITE(8, IREG(0), ireg0 & 0x01);
        MEMORY_WRITE(8, IREG(1), ireg1 & 0x7F);
        MEMORY_WRITE(8, IREG(2), 0xF0);

        MEMORY_WRITE(8, SMPC(COMREG), SMPC_SMC_INTBACK);

        return 0;
}

/// @brief Call SMPC command to turn master CPU on.
///
/// @warning Do not use this function.
static inline uint8_t __always_inline
smpc_smc_mshon_call(void)
{
        return smpc_smc_call(SMPC_SMC_MSHON);
}

/// @brief Call SMPC command to send an NMI request to the master CPU.
static inline uint8_t __always_inline
smpc_smc_nmireq_call(void)
{
        return smpc_smc_call(SMPC_SMC_NMIREQ);
}

/// @brief SMPC command to acquire SMPC status and peripheral data.
static inline uint8_t __always_inline
smpc_smc_resdisa_call(void)
{
        return smpc_smc_call(SMPC_SMC_RESDISA);
}

/// @brief Call SMPC command to enable NMI generation.
static inline uint8_t __always_inline
smpc_smc_resenab_call(void)
{
        return smpc_smc_call(SMPC_SMC_RESENAB);
}

/// @brief Call SMPC command to set data to SMEM.
static inline uint8_t __always_inline
smpc_smc_setsmem_call(void)
{
        return smpc_smc_call(SMPC_SMC_SETSMEM);
}

/// @brief Call SMPC command to turn sound CPU off.
///
/// @warning Do not use this function.
static inline uint8_t __always_inline
smpc_smc_sndoff_call(void)
{
        return smpc_smc_call(SMPC_SMC_SNDOFF);
}

/// @brief Call SMPC command to turn sound CPU on.
///
/// @warning Do not use this function.
static inline uint8_t __always_inline
smpc_smc_sndon_call(void)
{
        return smpc_smc_call(SMPC_SMC_SNDON);
}

/// @brief Call SMPC command to turn slave CPU off.
///
/// @warning Do not use this function.
static inline uint8_t __always_inline
smpc_smc_sshoff_call(void)
{
        return smpc_smc_call(SMPC_SMC_SSHOFF);
}

/// @brief Call SMPC command to turn slave CPU on.
///
/// @warning Do not use this function.
static inline uint8_t __always_inline
smpc_smc_sshon_call(void)
{
        return smpc_smc_call(SMPC_SMC_SSHON);
}

/// @brief Call SMPC command to reset the system.
///
/// @note Predictably, never returns.
static inline void __always_inline __noreturn
smpc_smc_sysres_call(void)
{
        smpc_smc_wait(true);

        MEMORY_WRITE(8, SMPC(COMREG), SMPC_SMC_SYSRES);

        smpc_smc_wait(false);

        __builtin_unreachable();
}

/// @}

__END_DECLS

#endif /* !_YAUL_SMPC_SMC_H_ */
