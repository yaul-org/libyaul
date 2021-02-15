/*
 * Copyright (c) 2012-2019 Israel Jacquez
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

static scu_dsp_ihr_t _dsp_end_ihr = _default_ihr;

static inline uint32_t _ppaf_read(void);
static inline void _flags_update(uint32_t);

void
_internal_scu_dsp_init(void)
{
        /* Disable DSP END interrupt */
        scu_ic_mask_chg(SCU_IC_MASK_ALL, SCU_IC_MASK_DSP_END);

        scu_dsp_program_clear();

        scu_dsp_end_clear();

        scu_ic_ihr_set(SCU_IC_INTERRUPT_DSP_END, _dsp_end_handler);

        _overflow = false;
        _end = true;
}

void
scu_dsp_end_set(scu_dsp_ihr_t ihr)
{
        scu_ic_mask_chg(SCU_IC_MASK_ALL, SCU_IC_MASK_DSP_END);

        _dsp_end_ihr = _default_ihr;

        if (ihr != NULL) {
                _dsp_end_ihr = ihr;

                scu_ic_mask_chg(~SCU_IC_MASK_DSP_END, SCU_IC_MASK_NONE);
        }
}

void
scu_dsp_program_load(const void *program, uint32_t count)
{
        if (count == 0) {
                return;
        }

        const uint32_t clamped_count =
            (count < DSP_PROGRAM_WORD_COUNT) ? count : DSP_PROGRAM_WORD_COUNT;

        scu_dsp_program_pc_set(0);

        uint32_t * const program_p = (uint32_t *)program;

        for (uint32_t i = 0; i < clamped_count; i++) {
                MEMORY_WRITE(32, SCU(PPD), program_p[i]);
        }

        scu_dsp_program_pc_set(0);
}

void
scu_dsp_program_clear(void)
{
        /* Load and start program that clears Z, S, and C flags */
        static const uint32_t program[] = {
                0x00020000, /* CLR A */
                0x00001501, /* MOV #$01, PL */
                0x10000000, /* ADD */
                /* XXX: This might cause an issue, as it might generate an
                 *      interrupt. */
                0xF8000000  /* ENDI */
        };

        scu_dsp_program_load(&program[0], sizeof(program) / sizeof(*program));
        scu_dsp_program_start();
        scu_dsp_program_end_wait();

        scu_dsp_program_pc_set(0);

        /* Clear program RAM */
        for (uint32_t i = 0; i < DSP_PROGRAM_WORD_COUNT; i++) {
                MEMORY_WRITE(32, SCU(PPD), 0xF8000000);
        }

        scu_dsp_program_pc_set(0);
}

void
scu_dsp_program_pc_set(scu_dsp_pc_t pc)
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

scu_dsp_pc_t
scu_dsp_program_step(void)
{
        volatile uint32_t * const reg_ppaf = (volatile uint32_t *)SCU(PPAF);

        uint32_t ppaf_bits;
        ppaf_bits = *reg_ppaf;

        uint8_t pc;
        pc = ppaf_bits & 0xFF;

        *reg_ppaf = 0x00020000;

        _overflow = false;
        _end = false;

        while (true) {
                _flags_update (ppaf_bits);

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
        _ppaf_read();

        return _end;
}

void
scu_dsp_program_end_wait(void)
{
        while (!(scu_dsp_program_end()));
}

bool
scu_dsp_dma_busy(void)
{
        uint32_t ppaf_bits;
        ppaf_bits = _ppaf_read();

        return ((ppaf_bits & 0x00800000) != 0x00000000);
}

void
scu_dsp_dma_wait(void)
{
        while ((scu_dsp_dma_busy()));
}

void
scu_dsp_data_read(scu_dsp_ram_t ram_page, uint8_t offset, void *data, uint32_t count)
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

        uint32_t * const data_p = (uint32_t *)data;

        const uint8_t address = (ram_page & 0x03) << 6;

        MEMORY_WRITE(32, SCU(PDA), address | offset);

        for (uint32_t i = 0; i < count; i++) {
                data_p[i] = MEMORY_READ(32, SCU(PDD));
        }
}

void
scu_dsp_data_write(scu_dsp_ram_t ram_page, uint8_t offset, void *data, uint32_t count)
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

        uint8_t address;
        address = (ram_page & 0x03) << 6;

        MEMORY_WRITE(32, SCU(PDA), address | offset);

        uint32_t i;
        for (i = 0; i < count; i++) {
                MEMORY_WRITE(32, SCU(PDD), data_p[i]);
        }
}

void
scu_dsp_status_get(scu_dsp_status_t *status)
{
        if (status == NULL) {
                return;
        }

        const uint32_t ppaf_bits = _ppaf_read();

        uint32_t *status_p;
        status_p = (uint32_t *)status;

        *status_p = ppaf_bits;

        status->v = status->v || _overflow;
        status->e = status->e || _end;
}

static void
_dsp_end_handler(void)
{
        _ppaf_read();

        _dsp_end_ihr();
}

static void
_default_ihr(void)
{
}

static inline uint32_t
_ppaf_read(void)
{
        volatile uint32_t * const reg_ppaf = (volatile uint32_t *)SCU(PPAF);

        /* Read SCU(PPAF) as the program end interrupt flag is reset when
         * read */

        uint32_t ppaf_bits;
        ppaf_bits = *reg_ppaf;

        _flags_update(ppaf_bits);

        return ppaf_bits;
}

static inline void __always_inline
_flags_update(uint32_t ppaf_bits)
{
        _overflow = _overflow || ((ppaf_bits & 0x00080000) != 0x00000000);
        _end = _end || ((ppaf_bits & 0x00040000) != 0x00000000);
}
