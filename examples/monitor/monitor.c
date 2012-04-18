/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <stdbool.h>

#include <ic/ic.h>
#include <bus/cpu/cpu.h>
#include <dma/dma.h>
#include <bus/b/vdp1/vdp1.h>
#include <bus/b/vdp2/vdp2.h>
#include <smpc.h>
#include <smpc/smc.h>
#include <smpc/peripheral.h>

#include <monitor/monitor.h>

#include <stdio.h>

#define RGB(r, g, b)    (0x8000 | ((r) & 0x1f) | (((g) & 0x1f)  << 5) | (((b) & 0x1f) << 10))
#define BLCS_COL(x)     (0x0001FFFE + (x))

int
main(void)
{
        static char buf[2048];

        uint16_t blcs_color[] = {
                RGB(0, 0, 21)
        };

        vdp2_init();
        vdp2_tvmd_display_set(); /* Turn display ON */
        vdp2_tvmd_blcs_set(/* lcclmd = */ false, 3, BLCS_COL(0), blcs_color, 0);
        smpc_init();
        monitor_init();

        while (true) {
                vdp2_tvmd_vblank_in_wait();
                /* Send "INTBACK" "SMPC" command (300us after
                 * VBLANK-IN). Set to 15-byte mode; optimized. */
                smpc_smc_intback_call(0x00, 0x0A);
                vdp2_tvmd_vblank_out_wait();
                smpc_peripheral_parse();

                (void)sprintf(buf,
                    "PORT: %d        PORT: %d\n"
                    "TYPE: 0x%02X     TYPE: 0x%02X\n"
                    "SIZE: %dB       SIZE: %dB\n"
                    "DATA[ 0]: 0x%02X DATA[ 0]: 0x%02X\n"
                    "DATA[ 1]: 0x%02X DATA[ 1]: 0x%02X\n"
                    "DATA[ 2]: 0x%02X DATA[ 2]: 0x%02X\n"
                    "DATA[ 3]: 0x%02X DATA[ 3]: 0x%02X\n"
                    "DATA[ 4]: 0x%02X DATA[ 4]: 0x%02X\n"
                    "DATA[ 5]: 0x%02X DATA[ 5]: 0x%02X\n"
                    "DATA[ 6]: 0x%02X DATA[ 6]: 0x%02X\n"
                    "DATA[ 7]: 0x%02X DATA[ 7]: 0x%02X\n"
                    "DATA[ 8]: 0x%02X DATA[ 8]: 0x%02X\n"
                    "DATA[ 9]: 0x%02X DATA[ 9]: 0x%02X\n"
                    "DATA[10]: 0x%02X DATA[10]: 0x%02X\n"
                    "DATA[11]: 0x%02X DATA[11]: 0x%02X\n"
                    "DATA[12]: 0x%02X DATA[12]: 0x%02X\n"
                    "DATA[13]: 0x%02X DATA[13]: 0x%02X\n"
                    "DATA[14]: 0x%02X DATA[14]: 0x%02X\a",
                    smpc_peripheral_port1.info.port_no,
                    smpc_peripheral_port2.info.port_no,
                    smpc_peripheral_port1.info.type,
                    smpc_peripheral_port2.info.type,
                    smpc_peripheral_port1.info.size,
                    smpc_peripheral_port2.info.size,
                    smpc_peripheral_port1.info.data[0],
                    smpc_peripheral_port2.info.data[0],
                    smpc_peripheral_port1.info.data[1],
                    smpc_peripheral_port2.info.data[1],
                    smpc_peripheral_port1.info.data[2],
                    smpc_peripheral_port2.info.data[2],
                    smpc_peripheral_port1.info.data[3],
                    smpc_peripheral_port2.info.data[3],
                    smpc_peripheral_port1.info.data[4],
                    smpc_peripheral_port2.info.data[4],
                    smpc_peripheral_port1.info.data[5],
                    smpc_peripheral_port2.info.data[5],
                    smpc_peripheral_port1.info.data[6],
                    smpc_peripheral_port2.info.data[6],
                    smpc_peripheral_port1.info.data[7],
                    smpc_peripheral_port2.info.data[7],
                    smpc_peripheral_port1.info.data[8],
                    smpc_peripheral_port2.info.data[8],
                    smpc_peripheral_port1.info.data[9],
                    smpc_peripheral_port2.info.data[9],
                    smpc_peripheral_port1.info.data[10],
                    smpc_peripheral_port2.info.data[10],
                    smpc_peripheral_port1.info.data[11],
                    smpc_peripheral_port2.info.data[11],
                    smpc_peripheral_port1.info.data[12],
                    smpc_peripheral_port2.info.data[12],
                    smpc_peripheral_port1.info.data[13],
                    smpc_peripheral_port2.info.data[13],
                    smpc_peripheral_port1.info.data[14],
                    smpc_peripheral_port2.info.data[14]);
                    vt100_write(monitor, buf);
        }

        return 0;
}
