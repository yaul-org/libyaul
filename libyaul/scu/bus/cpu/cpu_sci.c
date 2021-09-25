/*
 * Copyright (c) 2012-2021 Israel Jacquez
 * See LICENSE for details.
 *
 * Nikita Sokolov <hitomi2500@mail.ru>
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <cpu/map.h>
#include <cpu/sci.h>

void
cpu_sci_init(void)
{
        MEMORY_WRITE(8, CPU(SCR), 0x00); /* Stop all */
        MEMORY_WRITE(8, CPU(SMR), 0x80); /* Sync mode, 8bit, no parity, no MP, 1/4 clock */
        MEMORY_WRITE(8, CPU(BRR), 0x00); /* Maximum baudrate */
        MEMORY_WRITE(8, CPU(SCR), 0x01); /* Internal clock output */
        MEMORY_WRITE(8, CPU(SCR), 0xF1); /* Interrupts on, RX/TX on, internal clock output */
}
