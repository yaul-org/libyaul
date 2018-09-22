/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _SCU_INTERNAL_H_
#define _SCU_INTERNAL_H_

#include <stdint.h>
#include <stddef.h>

#include <scu/dma.h>
#include <scu/map.h>

struct state_scu_dma_level {
        uint8_t level;

        union {
                uint32_t buffer[DMA_REG_BUFFER_WORD_COUNT];

                struct {
                        uint32_t dnr;
                        uint32_t dnw;
                        uint32_t dnc;
                        uint32_t dnad;
                        uint32_t dnmd;
                };
        } buffered_regs;

        void (*ihr)(void);
} __aligned(32);

#endif /* !_SCU_INTERNAL_H_ */
