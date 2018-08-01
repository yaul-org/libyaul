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

static inline uint32_t _read_ppaf(void);
static inline void _update_flags(uint32_t);

void
scu_dsp_init(void)
{
        /* Disable DSP END interrupt */
        scu_ic_mask_chg(IC_MASK_ALL, IC_MASK_DSP_END);

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

        scu_dsp_program_pc_set(0);

        uint32_t *program_p;
        program_p = (uint32_t *)program;

        uint32_t i;
        for (i = 0; i < count; i++) {
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
                0xF8000000  /* ENDI */
        };

        scu_dsp_program_load(&program[0], sizeof(program) / sizeof(*program));
        scu_dsp_program_start();
        scu_dsp_program_end_wait();

        scu_dsp_program_pc_set(0);

        /* Clear program RAM */
        uint32_t i;
        for (i = 0; i < DSP_PROGRAM_WORD_COUNT; i++) {
                MEMORY_WRITE(32, SCU(PPD), 0xF8000000);
        }

        scu_dsp_program_pc_set(0);
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

        _overflow = false;
        _end = false;

        while (true) {
                _update_flags (ppaf_bits);

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
        _read_ppaf();

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
        ppaf_bits = _read_ppaf();

        return ((ppaf_bits & 0x00800000) != 0x00000000);
}

void
scu_dsp_dma_wait(void)
{
        while ((scu_dsp_dma_busy()));
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

        uint8_t address;
        address = (ram_page & 0x03) << 6;

        MEMORY_WRITE(32, SCU(PDA), address | offset);

        uint32_t i;
        for (i = 0; i < count; i++) {
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

        uint8_t address;
        address = (ram_page & 0x03) << 6;

        MEMORY_WRITE(32, SCU(PDA), address | offset);

        uint32_t i;
        for (i = 0; i < count; i++) {
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
        _read_ppaf();

        _dsp_end_ihr();
}

static void
_default_ihr(void)
{
}

static inline uint32_t
_read_ppaf(void)
{
        volatile uint32_t *reg_ppaf;
        reg_ppaf = (volatile uint32_t *)SCU(PPAF);

        /* Read SCU(PPAF) as the program end interrupt flag is reset
         * when read */

        uint32_t ppaf_bits;
        ppaf_bits = *reg_ppaf;

        _update_flags(ppaf_bits);

        return ppaf_bits;
}

static inline void __attribute__ ((always_inline))
_update_flags(uint32_t ppaf_bits)
{
        _overflow = _overflow || ((ppaf_bits & 0x00080000) != 0x00000000);
        _end = _end || ((ppaf_bits & 0x00040000) != 0x00000000);
}
