/*
 * Copyright (c) 2001
 * See LICENSE for details.
 *
 * William A. Gatliff <bgat@billgatliff.com>
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <inttypes.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "gdb.h"
#include "sh2-704x.h"

#define IS_ALIGNED(t, addr,len)                                               \
        (((len) >= sizeof(uint ## t ## _t)) &&                                \
        (((uint32_t)(addr) % sizeof(uint ## t ## _t)) == 0))

#define GDB_RX_BUF_LEN  512

typedef union {
        uint32_t lbuf;
        uint16_t sbuf;
        char cbuf[sizeof(uint32_t)];
} lc_t;

/* Helpers */
static char *hex_buffer_to_mem(const char *, void *, size_t);
static char *parse_unsigned_long(char *, uint32_t *, int);
static char low_nibble_to_hex(char);
static int hex_digit_to_integer(char);
static int mem_to_hex_buffer(const void *, char *, size_t);
static uint8_t put_buf(const char *, int);
static void get_packet(char *);
static void put_packet(char, const char *, size_t);

/* GDB commands */
static void gdb_command_read_memory(uint32_t, uint32_t);
static void gdb_command_read_registers(struct cpu_registers *);
static void gdb_last_signal(int);

/* At a minimum, a stub is required to support the `g',
 * `G', `m', `M', `c', and `s' commands
 *
 * All other commands are optional */
void
gdb_monitor(struct cpu_registers *reg_file, int sigval)
{
        static char buf[GDB_RX_BUF_LEN];

        char *hargs;

        uint32_t n;
        uint32_t r;
        uint32_t addr;
        uint32_t length;
        uint32_t kind;
        uint32_t type;
        char *data;

        int err;

        gdb_last_signal(sigval);
        gdb_monitor_entry(reg_file);

        while (true) {
                hargs = &*buf;
                get_packet(hargs);

                switch (*hargs++) {
                case '?':
                        /* '?'
                         * Indicate the reason the target halted */
                        gdb_last_signal(sigval);
                        break;
                case 'A':
                        /* 'A arg-len,arg-num,arg,...' *
                         * Initialized argv[] array passed into
                         * program */
                case 'c':
                        /* 'c [addr]'
                         * Continue */
                        return;
                case 'C':
                        /* 'C sig[;addr]'
                         * Continue with signal */
                        break;
                case 'D':
                        /* 'D'
                         * Detach gdb from the remote system */

                        put_packet('\0', "OK", 2);
                        return;
                case 'g':
                        /* 'g'
                         * Read general registers */
                        gdb_command_read_registers(reg_file);
                        break;
                case 'G':
                        /* 'G XX...'
                         * Write general registers */
                        break;
                case 'i':
                        /* 'i [addr[,nnn]]'
                         * Step the remote target by a single clock cycle */
                        break;
                case 'I':
                        /* 'I'
                         * Signal then cycle step */
                        break;
                case 'k':
                        /* 'k'
                         * Kill request
                         */

                        /* Send OK back to GDB */
                        put_packet('\0', "OK", 2);

                        gdb_kill();
                        return;
                case 'm':
                        /* 'm addr,length'
                         * Read LENGTH bytes memory from specified ADDRess */
                        hargs = parse_unsigned_long(hargs, &addr, ',');
                        parse_unsigned_long(hargs, &length, '\0');

                        gdb_command_read_memory(addr, length);
                        break;
                case 'M':
                        /* 'M addr,length:XX...'
                         * Write LENGTH bytes memory from ADDRess */

                        hargs = parse_unsigned_long(hargs, &addr, ',');
                        data = parse_unsigned_long(hargs, &length, ':');

                        hex_buffer_to_mem(data, (void *)addr, length);

                        /* Send OK back to GDB */
                        put_packet('\0', "OK", 2);
                        break;
                case 'P':
                        /* 'P n...=r...'
                         * Write to a specific register with a value */

                        hargs = parse_unsigned_long(hargs, &n, '=');

                        hex_buffer_to_mem(hargs, &r, sizeof(uint32_t));
                        gdb_register_file_write(reg_file, n, r);

                        /* Send OK back to GDB */
                        put_packet('\0', "OK", 2);
                        break;
                case 'R':
                        /* 'R XX'
                         * Restart the program being debugged */
                        break;
                case 's':
                        /* 's [addr]'
                         * Single step */
                        addr = 0x00000000;
                        parse_unsigned_long(hargs, &addr, '\0');
                        gdb_step(reg_file, addr);
                        return;
                case 'S':
                        /* 'S sig[;addr]'
                         * Step with signal */
                        return;
                case 't':
                        /* 't addr:PP,MM'
                         * Search backwards */
                        break;
                case 'z':
                        /* 'z type,addr,kind'
                         * Remove breakpoint or watchpoint */

                        type = 0x00000000;
                        addr = 0x00000000;
                        hargs = parse_unsigned_long(hargs, &type, ',');
                        hargs = parse_unsigned_long(hargs, &addr, ',');
                        parse_unsigned_long(hargs, &kind, '\0');

                        if ((err = gdb_remove_break(type, addr, kind)) < 0) {
                                put_packet('E', "01", 2);
                                break;
                        }

                        put_packet('\0', "OK", 2);
                        break;
                case 'Z':
                        /* 'Z type,addr,kind'
                         * Insert breakpoint or watchpoint */

                        type = 0x00000000;
                        addr = 0x00000000;
                        hargs = parse_unsigned_long(hargs, &type, ',');
                        hargs = parse_unsigned_long(hargs, &addr, ',');
                        parse_unsigned_long(hargs, &kind, '\0');

                        if ((err = gdb_break(type, addr, kind)) < 0) {
                                put_packet('E', "01", 2);
                                break;
                        }

                        /* Send OK back to GDB */
                        put_packet('\0', "OK", 2);
                        break;
                default:
                        /* Invalid */
                        put_packet('\0', NULL, 0);
                        break;
                }
        }
}

static void
gdb_command_read_registers(struct cpu_registers *reg_file)
{
        char tx_buf[8];
        size_t tx_len;

        uint8_t csum;

        uint32_t n;
        uint32_t r;

        do {
                gdb_putc('$');

                csum = 0;
                /* Loop through all registers */
                for (n = 0; gdb_register_file_read(reg_file, n, &r); n++) {
                        tx_len = mem_to_hex_buffer(&r, tx_buf, sizeof(r));
                        /* Send to GDB and calculate checksum */
                        csum += put_buf(tx_buf, tx_len);
                }

                /* Send message footer */
                gdb_putc('#');
                gdb_putc(low_nibble_to_hex(csum >> 4));
                gdb_putc(low_nibble_to_hex(csum));
        } while ((gdb_getc() & 0x7F) != '+');
}

static void
gdb_command_read_memory(uint32_t address, uint32_t length)
{
        char tx_buf[8];
        size_t tx_len;

        uint8_t csum;

        uint8_t *p;
        uint8_t r;
        uint32_t i;

        do {
                gdb_putc('$');

                csum = 0x00;
                for (i = 0; i < length; i++) {
                        /* Read from memory */
                        p = (uint8_t *)(address + i);
                        r = *p;
                        tx_len = mem_to_hex_buffer(&r, tx_buf, 1);

                        /* Send to GDB and calculate checksum */
                        csum += put_buf(tx_buf, tx_len);
                }

                /* Send message footer */
                gdb_putc('#');
                gdb_putc(low_nibble_to_hex(csum >> 4));
                gdb_putc(low_nibble_to_hex(csum));
        } while ((gdb_getc() & 0x7F) != '+');
}

static void
gdb_last_signal(int sigval)
{
        char tx_buf[2];

        tx_buf[0] = low_nibble_to_hex(sigval >> 4);
        tx_buf[1] = low_nibble_to_hex(sigval);

        /* Send packet */
        put_packet('S', tx_buf, sizeof(tx_buf));
}

/* Packet format
 * $packet-data#checksum */
static void
put_packet(char c, const char *buf, size_t len)
{
        uint8_t csum;

        do {
                /* Send the header */
                gdb_putc('$');

                /* Send the message type, if specified */
                if (c != '\0')
                        gdb_putc(c);
                /* Send the data */
                csum = c + put_buf(buf, len);

                /* Send the footer */
                gdb_putc('#');

                /* Checksum */
                gdb_putc(low_nibble_to_hex(csum >> 4));
                gdb_putc(low_nibble_to_hex(csum));
        } while ((gdb_getc() & 0x7F) != '+');
}

/* Packet format
 * $<data>#<checksum> */
static void
get_packet(char *rx_buf)
{
        uint8_t ch;
        uint8_t csum;
        uint8_t xmit_csum;

        uint16_t len;
        uint16_t i;

        do {
                /* Wait around for start character, ignore all others */
                while (((ch = gdb_getc()) & 0x7F) != '$') {
                        if (ch == '') {
                                /* ETX (end of text) */
                                ;
                        }
                }

                csum = 0x00;
                xmit_csum = 0xFF;

                /* now, read until a # or end of buffer is found */
                for (len = 0; ; len++) {
                        ch = gdb_getc() & 0x7F;
                        if (ch == '#')
                                break;

                        csum += ch;
                        rx_buf[len] = ch;
                }
                /* NULL terminate */
                rx_buf[len] = '\0';

                /* Check if packet has a correct checksum */
                xmit_csum = hex_digit_to_integer(gdb_getc() & 0x7F);
                xmit_csum = hex_digit_to_integer(gdb_getc() & 0x7F) + (xmit_csum << 4);

                if (csum != xmit_csum) {
                        /* Bad checksum */
                        gdb_putc('-');
                        continue;
                }
                /* Good checksum */
                gdb_putc('+');

                /* If a sequence char is present, reply with the
                 * sequence ID */
                /* Packet format
                 * $sequence-id:packet-data#checksum */
                if (rx_buf[2] == ':') {
                        assert(0);
                        gdb_putc(rx_buf[0]);
                        gdb_putc(rx_buf[1]);

                        /* Skip the first 3 characters by removing the
                         * sequence characters */
                        for (i = 3; i < len; i++)
                                rx_buf[i - 3] = rx_buf[i];
                }
        } while (false);
}

/* Convert '[0-9a-fA-F]' to its integer equivalent */
static int
hex_digit_to_integer(char h)
{

        if ((h >= 'a') && (h <= 'f'))
                return h - 'a' + 10;

        if ((h >= '0') && (h <= '9'))
                return h - '0';

        if ((h >= 'A') && (h <= 'F'))
                return h - 'A' + 10;

        return 0;
}

/* Convert the low nibble of C to its hexadecimal character
 * equivalent */
static char
low_nibble_to_hex(char c)
{
        static const char lut[] = {
                "0123456789ABCDEF"
        };

        return lut[c & 0x0F];
}

/* Translates a delimited hexadecimal string to an unsigned long (32-bit) */
static char *parse_unsigned_long(char *hargs, uint32_t *l, int delim)
{

        *l = 0x00000000;
        while (*hargs != delim)
                *l = (*l << 4) + hex_digit_to_integer(*hargs++);

        return hargs + 1;
}

/* Converts a memory region of length LEN bytes, starting at MEM, into a
 * string of hex bytes
 *
 * Returns the number of bytes placed into hexadecimal buffer
 *
 * This function carefully preserves the endianness of the data, because
 * that's what GDB expects */
static int
mem_to_hex_buffer(const void *mem, char *h_buf, size_t len)
{
        lc_t lc_buf;

        uint32_t i;
        size_t cbuf_len;
        int ret_val;

        ret_val = 0;
        for (i = 0; len > 0; ) {
                if (IS_ALIGNED(32, mem, len)) {
                        lc_buf.lbuf = *(uint32_t *)mem;
                        cbuf_len = sizeof(uint32_t);
                } else if (IS_ALIGNED(16, mem, len)) {
                        lc_buf.sbuf = *(uint16_t *)mem;
                        cbuf_len = sizeof(uint16_t);
                } else {
                        lc_buf.cbuf[0] = *(char*)mem;
                        cbuf_len = sizeof(char);
                }

                mem = (void *)((uint32_t)mem + cbuf_len);
                len -= cbuf_len;

                for (i = 0; i < cbuf_len; i++) {
                        *h_buf++ = low_nibble_to_hex(lc_buf.cbuf[i] >> 4);
                        *h_buf++ = low_nibble_to_hex(lc_buf.cbuf[i]);
                        ret_val += 2;
                }
        }

        return ret_val;
}

/* Reads twice the LEN hexadecimal digits from BUF and converts them to
 * binary
 *
 * Points to the first empty byte after the region written */
static char *
hex_buffer_to_mem(const char *buf, void *mem, size_t len)
{
        lc_t lc_buf;

        size_t cbuf_len;
        uint32_t i;

        for (i = 0; len > 0; ) {
                if (IS_ALIGNED(32, mem, len)) {
                        cbuf_len = sizeof(uint32_t);
                        for (i = 0; i < cbuf_len; i++) {
                                lc_buf.cbuf[i] = (hex_digit_to_integer(*buf++) << 4);
                                lc_buf.cbuf[i] += hex_digit_to_integer(*buf++);
                        }
                        *(uint32_t *)mem = lc_buf.lbuf;
                } else if (IS_ALIGNED(16, mem, len)) {
                        cbuf_len = sizeof(uint16_t);
                        for (i = 0; i < cbuf_len; i++ ) {
                                lc_buf.cbuf[i] = (hex_digit_to_integer(*buf++) << 4);
                                lc_buf.cbuf[i] += hex_digit_to_integer(*buf++);
                        }
                        *(uint16_t *)mem = lc_buf.sbuf;
                } else {
                        lc_buf.cbuf[0] = (hex_digit_to_integer(*buf++) << 4);
                        lc_buf.cbuf[0] += hex_digit_to_integer(*buf++);

                        cbuf_len = sizeof(char);
                        *(char *)mem = lc_buf.cbuf[0];
                }
                mem = (void *)((uint32_t)mem + cbuf_len);
                len -= cbuf_len;
        }

        return mem;
}

static uint8_t
put_buf(const char *buf, int len)
{
        uint8_t sum;
        char c;

        sum = 0x00;
        while (len--) {
                c = *buf++;
                sum += c;
                gdb_putc(c);
        }

        return sum;
}
