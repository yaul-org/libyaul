/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_CPU_SYNC_H_
#define _YAUL_CPU_SYNC_H_

#include <sys/cdefs.h>

#include <stdint.h>

__BEGIN_DECLS

/// @defgroup CPU_SYNC CPU Synchronization
///
/// @details All functions, otherwise noted make use of an exclusive area in the
/// first `0x4000` bytes of HWRAM for all the lock variables. The size of the
/// "lock array" is 256 bytes, and each lock variable is 1-byte.
///
/// For the lock array, the user is responsible for keeping track of usage.
/// There is no way to determine which locks are being used.

/// @addtogroup CPU_SYNC
/// @{

/// @brief An 8-bit integer value representing the lock index in the BIOS lock
/// array.
typedef uint8_t cpu_sync_lock_t;

/// @brief Provide functionality of mutual exclusion.
///
/// @details Implementation of the mutex is using test-and-set CPU instruction
/// `tas.b`.
///
/// @param b The lock index in the lock array.
///
/// @returns `true` if locking was previously unlocked, otherwise `false`.
static inline bool __always_inline
cpu_sync_mutex(cpu_sync_lock_t b)
{
        register uint8_t *bios_address;
        bios_address = (uint8_t *)HWRAM_UNCACHED(0x00000B00);

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

/// @brief Clear the lock.
///
/// @warning The clearing of @p b is not atomic.
///
/// @param b The lock index in the lock array.
static inline void __always_inline
cpu_sync_mutex_clear(cpu_sync_lock_t b)
{
        MEMORY_WRITE(8, HWRAM_UNCACHED(0x00000B00 + b), 0x00);
}

/// @brief Provide functionality of a spinlock.
///
/// @details Busy wait in a loop while repeatedly checking if the @p b is
/// available.
///
/// @param b The lock index in the lock array.
static inline void __always_inline
cpu_sync_spinlock(cpu_sync_lock_t b)
{
        register uint8_t *bios_address;
        bios_address = (uint8_t *)HWRAM_UNCACHED(0x00000B00);

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

/// @brief Clear the lock.
///
/// @warning The clearing of @p b is not atomic.
///
/// @param b The lock index in the lock array.
static inline void __always_inline
cpu_sync_spinlock_clear(cpu_sync_lock_t b)
{
        cpu_sync_mutex_clear(b);
}

/// @}

__END_DECLS

#endif /* !_YAUL_CPU_SYNC_H_ */
