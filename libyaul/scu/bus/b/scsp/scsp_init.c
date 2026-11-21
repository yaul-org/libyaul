/*
 * Copyright (c) Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <scsp.h>

#include <smpc/smc.h>

#include "scsp-internal.h"

/* Busy-wait iterations after halting the sound CPU. The SMPC asserts the
 * halt when it completes the sound off command, but the sound CPU can
 * finish one more bus cycle before it stops; a 68EC000 bus cycle takes well
 * under a microsecond. 500000 iterations take about 0.1 s at 28.6 MHz, and
 * still exceed 10 ms if the loop runs ten times slower than estimated.
 * Follow-up: measure the halt latency on hardware and shrink this constant
 * if it is confirmed much shorter. */
#define SOUND_CPU_HALT_SETTLE_LOOPS (500000UL)

void
scsp_init(void)
{
    /* Halt the sound CPU, so its program (the BIOS sound driver) stops
     * rewriting SCSP registers and sound RAM. Blocks until the SMPC has
     * completed the command. */
    smpc_smc_sndoff_call();

    for (volatile uint32_t i = 0; i < SOUND_CPU_HALT_SETTLE_LOOPS; i++) {
    }

    __scsp_init();
}

void
__scsp_init(void)
{
    /* Disable all sound CPU interrupts (SCIEB[10:0]) */
    scsp_scieb_set(0x0000);

    /* Disable all main CPU interrupts (MCIEB[10:0]) */
    scsp_mcieb_set(0x0000);

    /* Clear all sound CPU interrupt pending bits (SCIRE[10:0]) */
    scsp_scire_set(0x07FF);

    /* Clear all main CPU interrupt pending bits (MCIRE[10:0]) */
    scsp_mcire_set(0x07FF);
}
