/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_SCU_DSP_H_
#define _YAUL_SCU_DSP_H_

#include <sys/cdefs.h>

#include <stdint.h>

#include <scu/map.h>

__BEGIN_DECLS

/// @defgroup SCU_DSP SCU DSP

/// @addtogroup SCU_DSP
/// @{

/// @brief SCU-DSP RAM pages.
typedef enum scu_dsp_ram {
        /// SCU-DSP RAM page #0.
        DSP_RAM_PAGE_0 = 0x00,
        /// SCU-DSP RAM page #1.
        DSP_RAM_PAGE_1 = 0x01,
        /// SCU-DSP RAM page #2.
        DSP_RAM_PAGE_2 = 0x02,
        /// SCU-DSP RAM page #3.
        DSP_RAM_PAGE_3 = 0x03
} scu_dsp_ram_t;

/// SCU-DSP program size in bytes.
#define DSP_PROGRAM_SIZE        0x0400
/// SCU-DSP program word count.
#define DSP_PROGRAM_WORD_COUNT  (DSP_PROGRAM_SIZE / 4)

/// SCU-DSP RAM page size in bytes.
#define DSP_RAM_PAGE_SIZE       0x0100
/// SCU-DSP RAM page word count.
#define DSP_RAM_PAGE_WORD_COUNT (DSP_RAM_PAGE_SIZE / 4)

/// @brief Callback type.
typedef void (*scu_dsp_ihr_t)(void);

/// @brief Program counter to reference 256 instructions.
typedef uint8_t scu_dsp_pc_t;

/// @brief SCU-DSP status.
typedef struct scu_dsp_status {
        unsigned int :5;
        unsigned int :1;
        unsigned int :1;
        unsigned int :1;
        /// D0-Bus DMA execution flag.
        bool t0:1;
        /// SCU-DSP @em S (sine) flag.
        bool s:1;
        /// SCU-DSP @em Z (zero) flag.
        bool z:1;
        /// SCU-DSP @em C (carry) flag.
        bool c:1;
        /// SCU-DSP @em V (overflow) flag.
        bool v:1;
        /// SCU-DSP @em E (program end interrupt) flag.
        bool e:1;
        unsigned int :1;
        /// Flag to indicate whether execution is in progress or stopped.
        bool ex:1;
        unsigned int :1;
        unsigned int :7;
        /// Not yet documented.
        scu_dsp_pc_t pc:8;
} __packed scu_dsp_status_t;

/// @brief Pause SCU-DSP execution.
///
/// @param pause Pause or unpause.
static inline void __always_inline
scu_dsp_program_pause(bool pause)
{
        if (pause) {
                MEMORY_WRITE(32, SCU(PPAF), 0x02010000);
        } else {
                MEMORY_WRITE(32, SCU(PPAF), 0x04010000);
        }
}

/// @ingroup SCU_IC_HELPERS
/// @brief Clear the interrupt handler for the SCU-DSP end interrupt.
/// @see scu_dsp_end_clear
#define scu_dsp_end_clear()                                                    \
do {                                                                           \
        scu_dsp_end_set(NULL);                                                 \
} while (false)

/// @ingroup SCU_IC_HELPERS
/// @brief Set the interrupt handler for the SCU-DSP end interrupt.
///
/// @details There is no need to explicitly return via `rte` for @p ihr.
///
/// @param ihr The interrupt handler.
///
/// @see scu_dsp_end_clear
extern void scu_dsp_end_set(scu_dsp_ihr_t ihr);

/// @brief Load program onto SCU-DSP.
///
/// @details The PC is reset back to zero after loading the program. Should the
/// @p count be zero, nothing is copied. If the @p count exceeds @ref
/// DSP_PROGRAM_WORD_COUNT, the @p count is clamped.
///
/// Does not start SCU-DSP execution. For that, use @ref scu_dsp_program_start.
///
/// @param program Pointer to buffer containing the SCU-DSP program.
/// @param count   Number of instructions.
extern void scu_dsp_program_load(const void *program, uint32_t count);

/// @brief Completely clear program memory.
///
/// @details This function is heavy handed. The following is performed: Program
/// memory is cleared, the Z, S, and C flags are cleared, and the PC is reset.
extern void scu_dsp_program_clear(void);

/// @brief Set the SCU-DSP program counter.
///
/// @param pc The program counter.
extern void scu_dsp_program_pc_set(scu_dsp_pc_t pc);

/// @brief Start SCU-DSP execution.
extern void scu_dsp_program_start(void);

/// @brief Stop SCU-DSP execution.
extern void scu_dsp_program_stop(void);

/// @brief Step one instruction excecution.
///
/// @details The function blocks until that execution of one instruction is
/// complete.
///
/// @warning There is a possibility that this function never returns should the
/// SCU-DSP crash for any reason, or there is an instruction that causes the
/// state on the SCU to never update.
///
/// This function should be used to debug on real hardware.
extern scu_dsp_pc_t scu_dsp_program_step(void);

/// @brief Determine if the SCU-DSP DMA transfer is occurring.
/// @returns `true` if SCU-DSP DMA transfer is occurring. Otherwise, `false`.
extern bool scu_dsp_dma_busy(void);

/// @brief Wait until a SCU-DSP DMA transfer is complet.
extern void scu_dsp_dma_wait(void);

/// @brief Determine if SCU-DSP program has ended.
/// @returns `true` if the SCU-DSP has ended. Otherwise, `false`.
extern bool scu_dsp_program_end(void);

/// @brief Wait until the SCU-DSP program has ended.
extern void scu_dsp_program_end_wait(void);

/// @brief Read from one of the SCU-DSP RAM page.
///
/// @details If @p offset + @p count exceeds @ref DSP_RAM_PAGE_WORD_COUNT, the
/// function returns without copying.
///
/// @param      ram_page The RAM page.
/// @param      offset   The offset in the RAM page.
/// @param[out] data     The pointer to the buffer to write to.
/// @param      count    The number of words.
extern void scu_dsp_data_read(scu_dsp_ram_t ram_page, uint8_t offset,
    void *data, uint32_t count);

/// @brief Write to one of the SCU-DSP RAM page.
///
/// @details If @p offset + @p count exceeds @ref DSP_RAM_PAGE_WORD_COUNT, the
/// function returns without copying.
/// 
/// @param      ram_page The RAM page.
/// @param      offset   The offset in the RAM page.
/// @param[in]  data     The pointer to the buffer to write to.
/// @param      count    The number of words.
extern void scu_dsp_data_write(scu_dsp_ram_t ram_page, uint8_t offset,
    void *data, uint32_t count);

/// @brief Obtain SCU-DSP operation status.
///
/// @details If @p status is `NULL`, the status will not be updated.
///
/// @param[out] status The pointer to @ref scu_dsp_status_t.
extern void scu_dsp_status_get(scu_dsp_status_t *status);

/// @}

__END_DECLS

#endif /* !_YAUL_SCU_DSP_H_ */
