/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LDSPENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _SCU_DSP_H_
#define _SCU_DSP_H_

#include <sys/cdefs.h>

#include <stdint.h>

#include <scu/map.h>

__BEGIN_DECLS

/// @defgroup SCU_DSP_DEFINES
/// @defgroup SCU_DSP_STRUCTURES
/// @defgroup SCU_DSP_INLINE_FUNCTIONS
/// @defgroup SCU_DSP_HELPERS
/// @defgroup SCU_DSP_FUNCTIONS

/// @addtogroup SCU_DSP_DEFINES
/// @{

/// Not yet documented.
#define DSP_RAM_PAGE_0  0x00
/// Not yet documented.
#define DSP_RAM_PAGE_1  0x01
/// Not yet documented.
#define DSP_RAM_PAGE_2  0x02
/// Not yet documented.
#define DSP_RAM_PAGE_3  0x03

/// Not yet documented.
#define DSP_PROGRAM_SIZE        0x0400
/// Not yet documented.
#define DSP_PROGRAM_WORD_COUNT  (DSP_PROGRAM_SIZE / 4)

/// Not yet documented.
#define DSP_RAM_PAGE_SIZE       0x0100
/// Not yet documented.
#define DSP_RAM_PAGE_WORD_COUNT (DSP_RAM_PAGE_SIZE / 4)

/// @}

/// @addtogroup SCU_DSP_STRUCTURES
/// @{

/// @compound
/// @brief Not yet documented.
struct dsp_status {
        unsigned int :5;
        unsigned int :1;
        unsigned int :1;
        unsigned int :1;
        /// Not yet documented.
        unsigned int t0:1;
        /// Not yet documented.
        unsigned int s:1;
        /// Not yet documented.
        unsigned int z:1;
        /// Not yet documented.
        unsigned int c:1;
        /// Not yet documented.
        unsigned int v:1;
        /// Not yet documented.
        unsigned int e:1;
        unsigned int :1;
        /// Not yet documented.
        unsigned int ex:1;
        unsigned int :1;
        unsigned int :7;
        /// Not yet documented.
        unsigned int pc:8;
} __packed;

/// @}

/// @addtogroup SCU_DSP_INLINE_FUNCTIONS
/// @{

static inline void __always_inline
scu_dsp_program_pause(bool pause)
{
        if (pause) {
                MEMORY_WRITE(32, SCU(PPAF), 0x02010000);
        } else {
                MEMORY_WRITE(32, SCU(PPAF), 0x04010000);
        }
}

/// @}

/// @addtogroup SCU_DSP_HELPERS
/// @{

/// @brief Not yet documented.
#define scu_dsp_end_clear() do {                                               \
        scu_dsp_end_set(NULL);                                                 \
} while (false)

/// @}

/// @addtogroup SCU_DSP_FUNCTIONS
/// @{

/// @brief Not yet documented.
extern void scu_dsp_init(void);

/// @brief Not yet documented.
extern void scu_dsp_end_set(void (*)(void));

/// @brief Not yet documented.
extern void scu_dsp_program_load(const void *, uint32_t);

/// @brief Not yet documented.
extern void scu_dsp_program_clear(void);

/// @brief Not yet documented.
extern void scu_dsp_program_pc_set(uint8_t);

/// @brief Not yet documented.
extern void scu_dsp_program_start(void);

/// @brief Not yet documented.
extern void scu_dsp_program_stop(void);

/// @brief Not yet documented.
extern uint8_t scu_dsp_program_step(void);

/// @brief Not yet documented.
extern bool scu_dsp_dma_busy(void);

/// @brief Not yet documented.
extern void scu_dsp_dma_wait(void);

/// @brief Not yet documented.
extern bool scu_dsp_program_end(void);

/// @brief Not yet documented.
extern void scu_dsp_program_end_wait(void);

/// @brief Not yet documented.
extern void scu_dsp_data_read(uint8_t, uint8_t, void *, uint32_t);

/// @brief Not yet documented.
extern void scu_dsp_data_write(uint8_t, uint8_t, void *, uint32_t);

/// @brief Not yet documented.
extern void scu_dsp_status_get(struct dsp_status *);

/// @}

__END_DECLS

#endif /* !_SCU_DSP_H_ */
