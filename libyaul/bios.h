/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_BIOS_H_
#define _YAUL_BIOS_H_

#include <sys/cdefs.h>

#include <stdint.h>

__BEGIN_DECLS

#define CPU_CLOCK_SPEED_26MHZ   0
#define CPU_CLOCK_SPEED_28MHZ   1

static inline uint8_t * __always_inline
bios_power_persistent_memory_get(void)
{
        uint8_t *bios_address;
        bios_address = (uint8_t *)0x06000210;

        return bios_address;
}

static inline void __always_inline
bios_clock_speed_chg(uint8_t mode)
{
        register uint32_t *bios_address;
        bios_address = (uint32_t *)0x06000320;

        ((void (*)(uint8_t))*bios_address)(mode);
}

static inline uint32_t __always_inline
bios_clock_speed_get(void)
{
        register uint32_t *bios_address;
        bios_address = (uint32_t *)0x06000324;

        return *bios_address;
}

static inline void __noreturn __always_inline
bios_cd_player_execute(void)
{
        register uint32_t *bios_address;
        bios_address = (uint32_t *)0x0600026C;

        ((void (*)(void))*bios_address)();

        __builtin_unreachable();
}

static inline void __noreturn __always_inline
bios_execute(void)
{
        ((void (*)(void))0x00000420)();

        __builtin_unreachable();
}

__END_DECLS

#endif /* !_YAUL_BIOS_H_ */
