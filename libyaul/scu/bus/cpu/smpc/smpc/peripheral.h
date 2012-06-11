/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#ifndef _PERIPHERAL_H_
#define _PERIPHERAL_H_

#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>

#include <sys/queue.h>

#define MAX_PORT_DEVICES        6
#define MAX_PORT_DATA_SIZE      255

TAILQ_HEAD(multi_terminal, smpc_peripheral);

struct smpc_peripheral_analog {
        bool connected;
        /* If no children, port is 1 or 2. Otherwise, port is under multi-terminal */
        uint8_t port_no;
        uint8_t type;
        uint8_t size;

        struct {
                unsigned int right:1;
                unsigned int left:1;
                unsigned int down:1;
                unsigned int up:1;
                unsigned int start:1;
                unsigned int a_trg:1;
                unsigned int c_trg:1;
                unsigned int b_trg:1;

                unsigned int r_trg:1;
                unsigned int x_trg:1;
                unsigned int y_trg:1;
                unsigned int z_trg:1;
                unsigned int l_trg:1;

                uint8_t x;
                uint8_t y;
                uint8_t l_pdl;
                uint8_t r_pdl;
        } __attribute__ ((packed)) button;
} __attribute__ ((packed, __may_alias__));

struct smpc_peripheral_digital {
        bool connected;
        /* If no children, port is 1 or 2. Otherwise, port is under multi-terminal */
        uint8_t port_no;
        uint8_t type;
        uint8_t size;

        struct {
                unsigned int right:1;
                unsigned int left:1;
                unsigned int down:1;
                unsigned int up:1;
                unsigned int start:1;
                unsigned int a_trg:1;
                unsigned int c_trg:1;
                unsigned int b_trg:1;

                unsigned int r_trg:1;
                unsigned int x_trg:1;
                unsigned int y_trg:1;
                unsigned int z_trg:1;
                unsigned int l_trg:1;
        } __attribute__ ((packed)) button;
} __attribute__ ((packed, __may_alias__));

struct smpc_peripheral_info {
        bool connected;
        /* If no children, port is 1 or 2. Otherwise, port is under multi-terminal */
        uint8_t port_no;
        uint8_t type;
        uint8_t size;
        uint8_t data[MAX_PORT_DATA_SIZE]; /* Peripheral data table */
} __attribute__ ((packed));

struct smpc_peripheral {
        struct smpc_peripheral_info info;
        struct smpc_peripheral_port *parent;

        TAILQ_ENTRY(smpc_peripheral) peripherals;
};

struct smpc_peripheral_port {
        struct smpc_peripheral_info info;
        struct multi_terminal *children;
};

extern void smpc_peripheral_data(void);
extern void smpc_peripheral_parse(void);
extern void smpc_peripheral_system_manager(void);
struct smpc_peripheral_digital *smpc_peripheral_digital_port(uint8_t);

extern struct smpc_peripheral_port smpc_peripheral_port1;
extern struct smpc_peripheral_port smpc_peripheral_port2;

#endif /* !_PERIPHERAL_H_ */
