/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_CPU_DUAL_H_
#define _YAUL_CPU_DUAL_H_

#include <stdint.h>

#include <scu/map.h>
#include <cpu/frt.h>
#include <cpu/registers.h>

__BEGIN_DECLS

/// @defgroup CPU_DUAL CPU Dual

/// @addtogroup CPU_DUAL
/// @{

/// ID for which CPU.
typedef enum cpu_which {
        /// ID for master CPU
        CPU_MASTER,
        /// ID for slave CPU.
        CPU_SLAVE,
} cpu_which_t;

/// The communication mode used for the slave CPU.
typedef enum cpu_dual_comm_mode {
        /// @brief Use polling for master/slave CPU communication.
        /// @details The slave CPU directly polls for a notification from the
        /// master CPU.
        CPU_DUAL_ENTRY_POLLING,
        /// @brief Use the CPU-FRT ICI interrupt for master/slave CPU communication.
        /// @details An interrupt is fired when the master CPU sends a
        /// notification to the slave CPU.
        CPU_DUAL_ENTRY_ICI
} cpu_dual_comm_mode_t;

/// Callback for master CPU entry point.
typedef void (*cpu_dual_master_entry)(void);
/// Callback for slave CPU entry point.
typedef void (*cpu_dual_slave_entry)(void);

/// @brief From the slave CPU, notify the master CPU.
///
/// @details Calling this function will send a notification to the master CPU,
/// which calls the entry handler set via @ref cpu_dual_master_set.
///
/// Nothing happens should the master CPU entry handler be `NULL`
///
/// The effects of multiple calls to this function while the master CPU entry
/// handler is executing may differ depending on the communication mode set by
/// @ref cpu_dual_comm_mode_set.
///
/// @see cpu_dual_master_set
/// @see cpu_dual_master_clear
static inline void __always_inline
cpu_dual_master_notify(void)
{
        MEMORY_WRITE(16, SINIT, 0xFFFF);
}

/// @brief From the master CPU, notify the slave CPU.
///
/// @details Calling this function will send a notification to the slave CPU,
/// which calls the entry handler set via @ref cpu_dual_slave_set.
///
/// Nothing happens should the slave CPU entry handler be `NULL`
///
/// The effects of multiple calls to this function while the slave CPU entry
/// handler is executing may differ depending on the communication mode set by
/// @ref cpu_dual_comm_mode_set.
///
/// @see cpu_dual_slave_set
/// @see cpu_dual_slave_clear
static inline void __always_inline
cpu_dual_slave_notify(void)
{
        MEMORY_WRITE(16, MINIT, 0xFFFF);
}

/// @brief From either CPU, wait for a notification from the other CPU.
///
/// @details This function blocks until a notification from the other CPU is
/// received.
static inline void __always_inline
cpu_dual_notification_wait(void)
{
        volatile uint8_t * const reg_ftcsr = (volatile uint8_t *)CPU(FTCSR);

        while ((*reg_ftcsr & 0x80) == 0x00) {
        }

        *reg_ftcsr &= ~0x80;
}

/// @brief Obtain the top of the master CPU stack.
///
/// @details The pointer the master CPU can be changed per application via
/// changing the `IP_MASTER_STACK_ADDR` variable in the application `Makefile`.
///
/// @note The stack grows downward toward zero. Be sure to also specify an
/// address on a 4-byte boundary.
///
/// @returns A pointer.
static inline void * __always_inline
cpu_dual_master_stack_get(void)
{
        extern uint32_t _master_stack;

        return (void *)&_master_stack;
}

/// @brief Obtain the top of the slave CPU stack.
///
/// @details The pointer the slave CPU can be changed per application via
/// changing the `IP_SLAVE_STACK_ADDR` variable in the application `Makefile`.
///
/// @note The stack grows downward toward zero. Be sure to also specify an
/// address on a 4-byte boundary.
/// 
/// @returns A pointer.
static inline void * __always_inline
cpu_dual_slave_stack_get(void)
{
        extern uint32_t _slave_stack;

        return (void *)&_slave_stack;
}

/// @ingroup CPU_INTC_HELPERS
/// @brief Clear the master CPU entry handler.
#define cpu_dual_master_clear()                                                \
do {                                                                           \
        cpu_dual_master_set(NULL);                                             \
} while (false)

/// @ingroup CPU_INTC_HELPERS
/// @brief Clear the slave CPU entry handler.
#define cpu_dual_slave_clear()                                                 \
do {                                                                           \
        cpu_dual_slave_set(NULL);                                              \
} while (false)

/// @brief Select or change the master-slave CPU communication mode.
///
/// @details Upon changing the communication @p mode, the slave CPU is
/// completely re-initialized.
///
/// @param mode The communication mode.
extern void cpu_dual_comm_mode_set(cpu_dual_comm_mode_t mode);

/// @ingroup CPU_INTC_HELPERS
/// @brief Set the entry handler for the master CPU.
///
/// @details When the master CPU calls @ref cpu_dual_master_notify, @p entry will
/// be invoked. The @p entry handler can be `NULL`
///
/// @param entry The entry handler.
///
/// @see cpu_dual_master_notify
/// @see cpu_dual_master_clear
extern void cpu_dual_master_set(cpu_dual_master_entry entry);

/// @ingroup CPU_INTC_HELPERS
/// @brief Set the entry handler for the slave CPU.
///
/// @details When the master CPU calls @ref cpu_dual_slave_notify, @p entry will
/// be invoked. The @p entry handler can be `NULL`
///
/// @param entry The entry handler.
///
/// @see cpu_dual_slave_notify
/// @see cpu_dual_slave_clear
extern void cpu_dual_slave_set(cpu_dual_slave_entry entry);

/// @brief Obtain which of the two CPUs this function was called on.
///
/// @details Currently, the way the which CPU is determined is by comparing the
/// stack pointers.
///
/// @returns Which CPU.
extern cpu_which_t cpu_dual_executor_get(void);

/// @}

__END_DECLS

#endif /* !_YAUL_CPU_DUAL_H_ */
