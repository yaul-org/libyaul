/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LDSPENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _SCU_DSP_H_
#define _SCU_DSP_H_

#include <sys/cdefs.h>

#include <stdint.h>

#include <scu/map.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define DSP_RAM_PAGE_0  0x00
#define DSP_RAM_PAGE_1  0x01
#define DSP_RAM_PAGE_2  0x02
#define DSP_RAM_PAGE_3  0x03

#define DSP_PROGRAM_SIZE        0x0400
#define DSP_PROGRAM_WORD_COUNT  (DSP_PROGRAM_SIZE / 4)

#define DSP_RAM_PAGE_SIZE       0x0100
#define DSP_RAM_PAGE_WORD_COUNT (DSP_RAM_PAGE_SIZE / 4)

struct dsp_status {
        unsigned int :5;
        unsigned int :1;
        unsigned int :1;
        unsigned int :1;
        unsigned int t0:1;
        unsigned int s:1;
        unsigned int z:1;
        unsigned int c:1;
        unsigned int v:1;
        unsigned int e:1;
        unsigned int :1;
        unsigned int ex:1;
        unsigned int :1;
        unsigned int :7;
        unsigned int pc:8;
} __packed;

static inline void __always_inline
scu_dsp_program_pause(bool pause)
{
        if (pause) {
                MEMORY_WRITE(32, SCU(PPAF), 0x02010000);
        } else {
                MEMORY_WRITE(32, SCU(PPAF), 0x04010000);
        }
}

#define scu_dsp_end_clear() do {                                               \
        scu_dsp_end_set(NULL);                                                 \
} while (false)

extern void scu_dsp_init(void);
extern void scu_dsp_end_set(void (*)(void));
extern void scu_dsp_program_load(const void *, uint32_t);
extern void scu_dsp_program_clear(void);
extern void scu_dsp_program_pc_set(uint8_t);
extern void scu_dsp_program_start(void);
extern void scu_dsp_program_stop(void);
extern uint8_t scu_dsp_program_step(void);
extern bool scu_dsp_dma_busy(void);
extern void scu_dsp_dma_wait(void);
extern bool scu_dsp_program_end(void);
extern void scu_dsp_program_end_wait(void);
extern void scu_dsp_data_read(uint8_t, uint8_t, void *, uint32_t);
extern void scu_dsp_data_write(uint8_t, uint8_t, void *, uint32_t);
extern void scu_dsp_status_get(struct dsp_status *);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !_SCU_DSP_H_ */
