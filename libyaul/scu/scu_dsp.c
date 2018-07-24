/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <scu/dsp.h>
#include <scu/ic.h>

#include <scu-internal.h>

void
scu_dsp_init(void)
{
        scu_dsp_program_stop();

        /* Disable DSP END interrupt */
        scu_ic_mask_chg(IC_MASK_ALL, IC_MASK_DSP_END);

        scu_ic_ihr_set(IC_INTERRUPT_DSP_END, NULL);
}

void
scu_dsp_program_load(const void *program, uint32_t count)
{
        if (count == 0) {
                return;
        }

        scu_dsp_program_stop();

        uint32_t *program_p;
        program_p = (uint32_t *)program;

        uint32_t i;
        for (i = 0; i < count; i++) {
                MEMORY_WRITE(32, SCU(PPD), program_p[i]);
        }
}

void
scu_dsp_data_read(uint8_t ram_page, uint8_t offset, void *data, uint32_t count)
{
        if (count == 0) {
                return;
        }

        if (data == NULL) {
                return;
        }

        if (((uint16_t)count + offset) > DSP_RAM_PAGE_WORD_COUNT) {
                return;
        }

        uint32_t *data_p;
        data_p = (uint32_t *)data;

        uint8_t addr;
        addr = (ram_page & 0x03) << 6;

        uint32_t i;
        for (i = 0; i < count; i++) {
                uint32_t addr_offset;
                addr_offset = (i + offset) & (DSP_RAM_PAGE_WORD_COUNT - 1);

                MEMORY_WRITE(32, SCU(PDA), addr | addr_offset);

                data_p[i] = MEMORY_READ(32, SCU(PDD));
        }
}

void
scu_dsp_data_write(uint8_t ram_page, uint8_t offset, void *data, uint32_t count)
{
        if (count == 0) {
                return;
        }

        if (data == NULL) {
                return;
        }

        if (((uint16_t)count + offset) > DSP_RAM_PAGE_WORD_COUNT) {
                return;
        }

        uint32_t *data_p;
        data_p = (uint32_t *)data;

        uint8_t addr;
        addr = (ram_page & 0x03) << 6;

        uint32_t i;
        for (i = 0; i < count; i++) {
                uint32_t addr_offset;
                addr_offset = (i + offset) & (DSP_RAM_PAGE_WORD_COUNT - 1);

                MEMORY_WRITE(32, SCU(PDA), addr | addr_offset);

                MEMORY_WRITE(32, SCU(PDD), data_p[i]);
        }
}
