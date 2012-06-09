/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <vdp2.h>
#include <smpc.h>
#include <smpc/peripheral.h>

#include <cons/vdp2.h>

#include <stdio.h>
#include <stdbool.h>

int
main(void)
{
#define IS_BUTTON_PRESSED(p, s) ((p) ? "[1;37;0;42m"s"[m" : "*")

        static char text[2048];

        uint16_t blcs_color[] = {
                0x9C00
        };

        struct smpc_peripheral_digital *pad;
        struct cons cons;

        vdp2_init();
        vdp2_tvmd_blcs_set(/* lcclmd = */ false, VRAM_ADDR_4MBIT(3, 0x1FFFE),
            blcs_color, 0);

        smpc_init();

        cons_vdp2_init(&cons);

        pad = (struct smpc_peripheral_digital *)&smpc_peripheral_port1.info;

        while (true) {
                vdp2_tvmd_vblank_in_wait();
                vdp2_tvmd_vblank_out_wait();

                (void)sprintf(text,
                    "[1;32;1;45mConsole (cons) example[m\n"
                    "\n"
                    "PORT: %d        PORT: %d\n"
                    "TYPE: 0x%02X     TYPE: 0x%02X\n"
                    "SIZE: %dB       SIZE: %dB\n"
                    "DATA[ 0]: [1;46m0x%02X[m DATA[ 0]: [1;37;1;45m0x%02X[m\n"
                    "DATA[ 1]: [1;37;1;45m0x%02X[m DATA[ 1]: [1;37;1;45m0x%02X[m\n"
                    "DATA[ 2]: [1;37;1;40m0x%02X[m DATA[ 2]: [1;37;1;45m0x%02X[m\n"
                    "DATA[ 3]: [1;34;1;43m0x%02X[m DATA[ 3]: [1;37;1;45m0x%02X[m\n"
                    "DATA[ 4]: [1;34;1;42m0x%02X[m DATA[ 4]: [1;37;1;45m0x%02X[m\n"
                    "DATA[ 5]: [1;37;1;41m0x%02X[m DATA[ 5]: [1;37;1;45m0x%02X[m\n"
                    "DATA[ 6]: [1;37;1;45m0x%02X[m DATA[ 6]: [1;37;1;45m0x%02X[m\n"
                    "DATA[ 7]: [1;37;1;45m0x%02X[m DATA[ 7]: [1;37;1;45m0x%02X[m\n"
                    "DATA[ 8]: [1;37;1;44m0x%02X[m DATA[ 8]: [1;37;1;44m0x%02X[m\n"
                    "DATA[ 9]: [1;31;1;44m0x%02X[m DATA[ 9]: [1;31;1;44m0x%02X[m\n"
                    "DATA[10]: [1;31;1;44m0x%02X[m DATA[10]: [1;31;1;44m0x%02X[m\n"
                    "DATA[11]: [1;31;1;44m0x%02X[m DATA[11]: [1;31;1;44m0x%02X[m\n"
                    "DATA[12]: [1;31;1;44m0x%02X[m DATA[12]: [1;31;1;44m0x%02X[m\n"
                    "DATA[13]: [1;31;1;44m0x%02X[m DATA[13]: [1;31;1;44m0x%02X[m\n"
                    "DATA[14]: [1;31;1;44m0x%02X[m DATA[14]: [1;31;1;44m0x%02X[m\n",
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

                if (pad->type == 0) {
                        (void)sprintf(text, "%s[2B%s%s%s%s%s%s%s%s%s%s%s%s%s",
                            text,
                            IS_BUTTON_PRESSED(!pad->button.left, "L"),
                            IS_BUTTON_PRESSED(!pad->button.up, "U"),
                            IS_BUTTON_PRESSED(!pad->button.right, "R"),
                            IS_BUTTON_PRESSED(!pad->button.down, "D"),
                            IS_BUTTON_PRESSED(!pad->button.x_trg, "X"),
                            IS_BUTTON_PRESSED(!pad->button.y_trg, "Y"),
                            IS_BUTTON_PRESSED(!pad->button.z_trg, "Z"),
                            IS_BUTTON_PRESSED(!pad->button.a_trg, "A"),
                            IS_BUTTON_PRESSED(!pad->button.b_trg, "B"),
                            IS_BUTTON_PRESSED(!pad->button.c_trg, "C"),
                            IS_BUTTON_PRESSED(!pad->button.start, "S"),
                            IS_BUTTON_PRESSED(!pad->button.l_trg, "L"),
                            IS_BUTTON_PRESSED(!pad->button.r_trg, "R"));
                }

                (void)sprintf(text, "%s[H", text);
                cons_write(&cons, text);
        }

        return 0;
}
