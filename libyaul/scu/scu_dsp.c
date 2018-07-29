/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <scu/dsp.h>
#include <scu/ic.h>

#include <scu-internal.h>

static void _dsp_end_handler(void);

static void _default_ihr(void);

/* XXX: State that should be moved (eventually) */
static bool _overflow = false;
static bool _end = true;

static void (*_dsp_end_ihr)(void) = _default_ihr;

void
scu_dsp_init(void)
{
        /* Disable DSP END interrupt */
        scu_ic_mask_chg(IC_MASK_ALL, IC_MASK_DSP_END);

        scu_dsp_program_stop();

        scu_dsp_program_clear();

        scu_dsp_end_clear();

        scu_ic_ihr_set(IC_INTERRUPT_DSP_END, _dsp_end_handler);

        _overflow = false;
        _end = true;
}

void
scu_dsp_end_set(void (*ihr)(void))
{
        scu_ic_mask_chg(IC_MASK_ALL, IC_MASK_DSP_END);

        _dsp_end_ihr = _default_ihr;

        if (ihr != NULL) {
                _dsp_end_ihr = ihr;

                scu_ic_mask_chg(~IC_MASK_DSP_END, IC_MASK_NONE);
        }
}

void
scu_dsp_program_load(const void *program, uint32_t count)
{
        if (count == 0) {
                return;
        }

        if (count > DSP_PROGRAM_WORD_COUNT) {
                return;
        }

        scu_dsp_program_stop();
        scu_dsp_program_pc_set(0);

        uint32_t *program_p;
        program_p = (uint32_t *)program;

        uint32_t i;
        for (i = 0; i < count; i++) {
                MEMORY_WRITE(32, SCU(PPD), program_p[i]);
        }
}

void
scu_dsp_program_clear(void)
{
        scu_dsp_program_stop();
        scu_dsp_program_pc_set(0);

        uint32_t i;
        for (i = 0; i < DSP_PROGRAM_WORD_COUNT; i++) {
                MEMORY_WRITE(32, SCU(PPD), 0xF8000000);
        }
}

void
scu_dsp_program_pc_set(uint8_t pc)
{
        MEMORY_WRITE(32, SCU(PPAF), 0x00008000 | pc);

        _overflow = false;
        _end = true;
}

void
scu_dsp_program_start(void)
{
        MEMORY_WRITE(32, SCU(PPAF), 0x00010000);

        _overflow = false;
        _end = false;
}

void
scu_dsp_program_stop(void)
{
        MEMORY_WRITE(32, SCU(PPAF), 0x00000000);

        _overflow = false;
        _end = true;
}

uint8_t
scu_dsp_program_step(void)
{
        volatile uint32_t *reg_ppaf;
        reg_ppaf = (volatile uint32_t *)SCU(PPAF);

        uint32_t ppaf_bits;
        ppaf_bits = *reg_ppaf;

        uint8_t pc;
        pc = ppaf_bits & 0xFF;

        *reg_ppaf = 0x00020000;

        while (true) {
                _overflow = (ppaf_bits & 0x00080000) != 0x00000000;

                /* Read SCU(PPAF) as the program end interrupt flag is
                 * reset when read */
                _end = (ppaf_bits & 0x00040000) != 0x00000000;

                /* Wait until DSP is done executing one instruction */
                if (_end || ((ppaf_bits & 0x00030000) == 0x00000000)) {
                        break;
                }

                ppaf_bits = *reg_ppaf;
        }

        return pc;
}

bool
scu_dsp_program_end(void)
{
        volatile uint32_t *reg_ppaf;
        reg_ppaf = (volatile uint32_t *)SCU(PPAF);

        uint32_t ppaf_bits;
        ppaf_bits = *reg_ppaf;

        _overflow = _overflow || ((ppaf_bits & 0x00080000) != 0x00000000);
        _end = _end || ((ppaf_bits & 0x00040000) != 0x00000000);

        return _end;
}

void
scu_dsp_program_end_wait(void)
{
        while (!(scu_dsp_program_end()));
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
                addr_offset = i + offset;

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
                addr_offset = i + offset;

                MEMORY_WRITE(32, SCU(PDA), addr | addr_offset);

                MEMORY_WRITE(32, SCU(PDD), data_p[i]);
        }
}

void
scu_dsp_status_get(struct dsp_status *status)
{
        if (status == NULL) {
                return;
        }

        volatile uint32_t *reg_ppaf;
        reg_ppaf = (volatile uint32_t *)SCU(PPAF);

        uint32_t ppaf_bits;
        ppaf_bits = *reg_ppaf;

        uint32_t *status_p;
        status_p = (uint32_t *)status;

        *status_p = ppaf_bits;

        status->v = status->v || _overflow;
        status->e = status->e || _end;
}

static void
_dsp_end_handler(void)
{
        /* Read SCU(PPAF) as the program end interrupt flag is reset
         * when read */
        MEMORY_WRITE_AND(32, SCU(PPAF), ~0x00040000);

        _end = true;

        _dsp_end_ihr();
}

static void
_default_ihr(void)
{
}
