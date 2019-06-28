/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _CPU_SYNC_H_
#define _CPU_SYNC_H_

#include <sys/cdefs.h>

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

static inline bool __always_inline
cpu_sync_mutex(uint8_t b)
{
        register uint8_t *bios_address;
        bios_address = (uint8_t *)0x26000B00;

        register uint32_t result;
        result = 0;

        /* Currently using cpu_sync_mutex() to compare the value results in
         * extra instructions being emitted. Because TAS.B sets the T bit in SR,
         * there shouldn't be a need for the MOVT instruction */

        __asm__ volatile ("\tadd %[b], %[bios_address]\n"
                          "\ttas.b @%[out]\n"
                          "\tmovt %[result]"
            /* Output */
            : [out] "=r" (bios_address),
              [result] "=r" (result)
            /* Input */
            : [bios_address] "r" (bios_address),
              [b] "r" (b));

        return result;
}

static inline void __always_inline
cpu_sync_mutex_clear(uint8_t b)
{
        register uint8_t *bios_address;
        bios_address = (uint8_t *)0x26000B00;

        *(bios_address + b) = 0x00;
}

static inline void __always_inline
cpu_sync_spinlock(uint8_t b)
{
        register uint8_t *bios_address;
        bios_address = (uint8_t *)0x26000B00;

        __asm__ volatile ("\tadd %[b], %[bios_address]\n"
                          "1:\n"
                          "\ttas.b @%[out]\n"
                          "\tbf 1b"
            /* Output */
            : [out] "=r" (bios_address)
            /* Input */
            : [bios_address] "0" (bios_address),
              [b] "r" (b));
}

static inline void __always_inline
cpu_sync_spinlock_clear(uint8_t b)
{
        cpu_sync_mutex_clear(b);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !_CPU_SYNC_H_ */
