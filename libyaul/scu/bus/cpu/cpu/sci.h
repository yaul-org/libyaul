/*
 * Copyright (c) Israel Jacquez
 * See LICENSE for details.
 *
 * Nikita Sokolov <hitomi2500@mail.ru>
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_CPU_SCI_H_
#define _YAUL_CPU_SCI_H_

#include <sys/cdefs.h>

#include <assert.h>
#include <stdint.h>
#include <stddef.h>

#include <cpu/map.h>

__BEGIN_DECLS

/// @defgroup CPU_SCI CPU Serial Communication Interface (SCI)

/// @addtogroup CPU_SCI
/// @{

/// @brief CPU-SCI communication mode.
/// @see cpu_sci_cfg_t
typedef enum cpu_sci_mode {
        /// @brief Asynchronous mode.
        CPU_SCI_MODE_ASYNC = 0x00,
        /// @brief Clocked synchronous mode.
        CPU_SCI_MODE_SYNC  = 0x01
} cpu_sci_mode_t;

/// @brief Single transfer length in bits.
/// @see cpu_sci_cfg_t
typedef enum cpu_sci_length {
        /// @brief 8-bit data.
        CPU_SCI_LENGTH_8_BITS = 0x00,
        /// @brief 7-bit data.
        /// @details When 7-bit data is selected, the MSB (bit 7) of the
        /// transmit data register is not transmitted.
        CPU_SCI_LENGTH_7_BITS = 0x01
} cpu_sci_length_t;

/// @brief Parity.
/// @see cpu_sci_cfg_t
typedef enum cpu_sci_parity {
        /// @brief Parity bit not added or checked.
        CPU_SCI_PARITY_OFF = 0x00,
        /// @brief Parity bit added and checked.
        /// @details When enabled, an even or odd parity bit is added to
        /// transmit data, depending on the parity mode setting. Receive data
        /// parity is checked according to the even/odd mode setting.
        CPU_SCI_PARITY_ON  = 0x01
} cpu_sci_parity_t;

/// @brief Parity mode.
/// @see cpu_sci_cfg_t
typedef enum cpu_sci_parity_mode {
        /// @brief Even parity.
        CPU_SCI_PARITY_EVEN = 0x00,
        /// @brief Odd parity.
        CPU_SCI_PARIT_ODD   = 0x01
} cpu_sci_parity_mode_t;

/// @brief Stop bit length.
/// @see cpu_sci_cfg_t
typedef enum cpu_sci_stop_bit_length {
        /// @brief One stop bit.
        CPU_SCI_1_STOP_BIT  = 0x00,
        /// @brief Two stop bits.
        CPU_SCI_2_STOP_BITS = 0x01
} cpu_sci_stop_bit_length_t;

/// @brief Multiprocessor mode.
/// @see cpu_sci_cfg_t
typedef enum cpu_sci_mp {
        /// @brief Multiprocessor function disabled.
        CPU_SCI_MP_OFF = 0x00,
        /// @brief Multiprocessor function enabled.
        CPU_SCI_MP_ON  = 0x01
} cpu_sci_mp_t;

/// @brief Clock divider for baudrate prescaler.
/// @see cpu_sci_cfg_t
typedef enum cpu_sci_clock_div {
        /// @brief 1/4 clock division.
        CPU_SCI_CLOCK_DIV_4   = 0x00,
        /// @brief 1/16 clock division.
        CPU_SCI_CLOCK_DIV_16  = 0x01,
        /// @brief 1/64 clock division.
        CPU_SCI_CLOCK_DIV_64  = 0x02,
        /// @brief 1/256 clock division.
        CPU_SCI_CLOCK_DIV_256 = 0x03
} cpu_sci_clock_div_t;

/// @brief SCK pin configuration.
/// @see cpu_sci_cfg_t
typedef enum cpu_sci_sck_cfg {
        /// @brief SCK pin is unused.
        CPU_SCI_SCK_DONTCARE = 0x00,
        /// @brief SCK pin is used as a clock output.
        CPU_SCI_SCK_OUTPUT   = 0x01,
        /// @brief SCK pin is used as a clock input.
        CPU_SCI_SCK_INPUT    = 0x02,
} cpu_sci_sck_cfg_t;

/// @brief Callback type.
/// @see cpu_sci_cfg_t.ihr_eri
/// @see cpu_sci_cfg_t.ihr_rxi
/// @see cpu_sci_cfg_t.ihr_txi
/// @see cpu_sci_cfg_t.ihr_tei
/// @see cpu_sci_cfg_t.ihr_work
typedef void (*cpu_sci_ihr)(void *ihr);

/// @brief CPU-SCI configuration.
typedef struct cpu_sci_cfg {
        /// Communication mode.
        cpu_sci_mode_t mode:1;

        /// Length.
        cpu_sci_length_t length:1;

        /// Parity.
        cpu_sci_parity_t parity:1;

        /// Parity mode.
        cpu_sci_parity_mode_t parity_mode:1;

        /// Stop bit.
        cpu_sci_stop_bit_length_t stop_bit:1;

        /// Multiprocessor mode.
        cpu_sci_mp_t mp:1;

        /// Clock divider.
        cpu_sci_clock_div_t clock_div:2;

        /// SCK pin configuration.
        cpu_sci_sck_cfg_t sck_config:2;

        /// Baudrate.
        uint8_t baudrate;

        /// @ingroup CPU_INTC_HELPERS
        /// @brief Callback when receive error is encountered.
        ///
        /// @details Set to `NULL` if no callback is desired.
        cpu_sci_ihr ihr_eri;

        /// @ingroup CPU_INTC_HELPERS
        /// @brief Callback when new serial data is received in receive data
        /// register.
        ///
        /// @details Set to `NULL` if no callback is desired.
        cpu_sci_ihr ihr_rxi;

        /// @ingroup CPU_INTC_HELPERS

        /// @brief Callback when transmit data register content is dispatched
        /// for transfer.
        ///
        /// @details Set to `NULL` if no callback is desired.
        cpu_sci_ihr ihr_txi;

        /// @ingroup CPU_INTC_HELPERS
        /// @brief Callback when transmit data register content is sent.
        ///
        /// @details Set to `NULL` if no callback is desired.
        cpu_sci_ihr ihr_tei;
} __aligned(4) cpu_sci_cfg_t;

static_assert(sizeof(cpu_sci_cfg_t) == 20);

/// @brief Enable CPU-SCI.
static inline void __always_inline
cpu_sci_enable(void)
{
        volatile cpu_ioregs_t * const cpu_ioregs = (volatile cpu_ioregs_t *)CPU_IOREG_BASE;

        cpu_ioregs->scr |= 0x30;
}

/// @brief Disable CPU-SCI.
static inline void __always_inline
cpu_sci_disable(void)
{
        volatile cpu_ioregs_t * const cpu_ioregs = (volatile cpu_ioregs_t *)CPU_IOREG_BASE;

        cpu_ioregs->scr &= ~0x30;
}

/// @brief Set the interrupt priority level for CPU-SCI.
///
/// @param priority The priority ranging from `0` to `15`.
static inline void __always_inline
cpu_sci_interrupt_priority_set(uint8_t priority)
{
        volatile cpu_ioregs_t * const cpu_ioregs = (volatile cpu_ioregs_t *)CPU_IOREG_BASE;

        cpu_ioregs->iprb &= 0x7FFF;
        cpu_ioregs->iprb |= (priority & 0x0F) << 12;
}

/// @brief Reset CPU-SCI status.
static inline void __always_inline
cpu_sci_status_reset(void)
{
        volatile cpu_ioregs_t * const cpu_ioregs = (volatile cpu_ioregs_t *)CPU_IOREG_BASE;

        cpu_ioregs->ssr = 0x00;
}

/// @brief Configure a CPU-SCI for transfer.
///
/// @details Configuring the CPU-SCI in @p cfg does not start the transfer. To
/// start the transfer, use either @ref cpu_sci_with_dmac_enable (for SCI+DMAC
/// mode) or @ref cpu_sci_write_value_set (for normal mode). The CPU-SCI is
/// forcefully stopped upon starting the configuration. If the CPU_SCI is
/// currently operating int SCI+DMAC mode, use @ref cpu_dmac_channel_wait with the
/// corresponding CPU-DMAC channel to wait until the transfer is complete.
///
/// @param[in] cfg The CPU-SCI transfer configuration.
///
/// @see cpu_sci_with_dmac_enable
/// @see cpu_sci_write_value_set
/// @see cpu_sci_read_value_get
void cpu_sci_config_set(const cpu_sci_cfg_t *cfg);

/// @brief Execute a CPU-SCI transfer in CPI+DMAC mode.
///
/// @details Every transfer should be configured with @ref cpu_sci_config_set
/// first. The corresponding DMAC channed should be started with @ref
/// cpu_dmac_channel_start as well.
///
/// @param[in] cfg The CPU-SCI transfer configuration.
///
/// @see cpu_sci_config_set
/// @see cpu_sci_write_value_set
/// @see cpu_sci_read_value_get
void cpu_sci_with_dmac_enable(const cpu_sci_cfg_t *cfg);

/// @brief Execute a single byte CPU-SCI transfer in normal mode.
///
/// @details Every transfer is bidirectional, reading is done alowng with
/// writing. To get the readen valuer call @ref cpu_sci_read_value_get.
///
/// @param[in] value Value for transmitting.
///
/// @see cpu_sci_config_set
/// @see cpu_sci_read_value_get
static inline void __always_inline
cpu_sci_write_value_set(uint8_t value)
{
        volatile cpu_ioregs_t * const cpu_ioregs = (volatile cpu_ioregs_t *)CPU_IOREG_BASE;

        cpu_ioregs->tdr = value;
}

/// @brief Returns the last value read during CPU-SCI transfer in normal mode.
///
/// @details This function returns the value readen diring last @ref
/// cpu_sci_write_value_set call.
///
/// @see cpu_sci_config_set
/// @see cpu_sci_write_value_set
static inline uint8_t __always_inline
cpu_sci_read_value_get(void)
{
        volatile cpu_ioregs_t * const cpu_ioregs = (volatile cpu_ioregs_t *)CPU_IOREG_BASE;

        const uint8_t value = cpu_ioregs->rdr;

        /* If RDRF is set, clear it */
        if ((cpu_ioregs->ssr & 0x40) > 0) {
                cpu_ioregs->ssr &= ~0x40;
        }

        return value;
}

/// @brief Waits for CPU-SCI transfer end.
///
/// @see cpu_sci_config_set
/// @see cpu_sci_value_write_set
static inline void __always_inline
cpu_sci_wait(void)
{
        volatile cpu_ioregs_t * const cpu_ioregs = (volatile cpu_ioregs_t *)CPU_IOREG_BASE;

        /* Waiting for TEND */
        while ((cpu_ioregs->ssr & 0x04) == 0) {
        }

        /* Waiting for RDRF */
        while ((cpu_ioregs->ssr & 0x40) == 0) {
        }
}

/// @}

__END_DECLS

#endif /* !_YAUL_CPU_SCI_H_ */
