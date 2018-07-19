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

static inline void __attribute__ ((always_inline))
cpu_sync_spinlock(uint8_t b)
{
        register uint8_t *bios_address;
        bios_address = (uint8_t *)0x26000B00;

        __asm__ volatile ("add %[b], %[bios_address]\n"
                          "1:\n"
                          "\ttas.b @%[out]\n"
                          "\tbf 1b"
            : [out] "=r" (bios_address)
            : [bios_address] "0" (bios_address), [b] "r" (b));
}

static inline void __attribute__ ((always_inline))
cpu_sync_spinlock_clear(uint8_t b)
{
        register uint8_t *bios_address;
        bios_address = (uint8_t *)0x26000B00;

        *(bios_address + b) = 0x00;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !_CPU_SYNC_H_ */
