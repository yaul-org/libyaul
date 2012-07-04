/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2.h>
#include <smpc.h>
#include <smpc/peripheral.h>

#include <cons/vdp2.h>

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

int
main(void)
{
#define IS_BUTTON_PRESSED(p, s) ((p) ? "[1;37;0;42m"s"[m" : "*")

        uint16_t blcs_color[] = {
                0x9C00
        };

        char *text;

        struct smpc_peripheral_digital *digital;
        struct smpc_peripheral_port *port1;
        struct smpc_peripheral_port *port2;

        struct cons cons;

        vdp2_init();
        vdp2_tvmd_blcs_set(/* lcclmd = */ false, VRAM_ADDR_4MBIT(3, 0x1FFFE),
            blcs_color, 0);

        smpc_init();

        cons_vdp2_init(&cons);

        port1 = smpc_peripheral_raw_port(1);
        port2 = smpc_peripheral_raw_port(2);
        digital = smpc_peripheral_digital_port(1);

        text = (char *)malloc(8192);
        assert(text != NULL);

        while (true) {
                vdp2_tvmd_vblank_in_wait();
                vdp2_tvmd_vblank_out_wait();

                (void)sprintf(text,
                    "[1;32;1;45m    *** Console (cons component) ***    [m\n"
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
                    port1->info.port_no, port2->info.port_no,
                    port1->info.type, port2->info.type,
                    port1->info.size, port2->info.size,
                    port1->info.data[0], port2->info.data[0],
                    port1->info.data[1], port2->info.data[1],
                    port1->info.data[2], port2->info.data[2],
                    port1->info.data[3], port2->info.data[3],
                    port1->info.data[4], port2->info.data[4],
                    port1->info.data[5], port2->info.data[5],
                    port1->info.data[6], port2->info.data[6],
                    port1->info.data[7], port2->info.data[7],
                    port1->info.data[8], port2->info.data[8],
                    port1->info.data[9], port2->info.data[9],
                    port1->info.data[10], port2->info.data[10],
                    port1->info.data[11], port2->info.data[11],
                    port1->info.data[12],port2->info.data[12],
                    port1->info.data[13], port2->info.data[13],
                    port1->info.data[14], port2->info.data[14]);

                if (digital->connected && (digital->type == 0)) {
                        (void)sprintf(text, "%s[2B%s%s%s%s%s%s%s%s%s%s%s%s%s",
                            text,
                            IS_BUTTON_PRESSED(!digital->button.left, "L"),
                            IS_BUTTON_PRESSED(!digital->button.up, "U"),
                            IS_BUTTON_PRESSED(!digital->button.right, "R"),
                            IS_BUTTON_PRESSED(!digital->button.down, "D"),
                            IS_BUTTON_PRESSED(!digital->button.x_trg, "X"),
                            IS_BUTTON_PRESSED(!digital->button.y_trg, "Y"),
                            IS_BUTTON_PRESSED(!digital->button.z_trg, "Z"),
                            IS_BUTTON_PRESSED(!digital->button.a_trg, "A"),
                            IS_BUTTON_PRESSED(!digital->button.b_trg, "B"),
                            IS_BUTTON_PRESSED(!digital->button.c_trg, "C"),
                            IS_BUTTON_PRESSED(!digital->button.start, "S"),
                            IS_BUTTON_PRESSED(!digital->button.l_trg, "L"),
                            IS_BUTTON_PRESSED(!digital->button.r_trg, "R"));
                } else
                        (void)sprintf(text, "%s[0J", text);

                (void)sprintf(text, "%s[H", text);
                cons_write(&cons, text);
        }

        return 0;
}
