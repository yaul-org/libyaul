/*
 * Copyright (c) 2012-2019
 * See LICENSE for details.
 *
 * William A. Gatliff <bgat@billgatliff.com>
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "gdb.h"

#include "sh2.inc"

#define IS_POW_2_ALIGNED(t, addr,len)                                          \
        (((len) >= sizeof(uint ## t ## _t)) &&                                 \
        (((uint32_t)(addr) & (sizeof(uint ## t ## _t) - 1)) == 0))

#define GDB_RX_BUF_LEN 512

typedef union {
        uint32_t lbuf;
        uint16_t sbuf;
        char cbuf[sizeof(uint32_t)];
} lc_t;

/* Helpers */
static char *_hex_buffer_to_mem(const char *, void *, size_t);
static char *_parse_unsigned_long(char *, uint32_t *, int);
static char _low_nibble_to_hex(char);
static int _hex_digit_to_integer(char);
static int _mem_to_hex_buffer(const void *, char *, size_t);
static uint8_t _put_buf(const char *, int);
static void _get_packet(char *);
static void _put_packet(char, const char *, size_t);

/* GDB commands */
static void _gdb_command_read_memory(uint32_t, uint32_t);
static void _gdb_command_read_registers(cpu_registers_t *);
static void _gdb_last_signal(int);

void
gdb_init(void)
{
        _gdb_sh2_init();
}

/* At a minimum, a stub is required to support the `g',
 * `G', `m', `M', `c', and `s' commands
 *
 * All other commands are optional */
void
gdb_monitor(cpu_registers_t *reg_file, int sigval)
{
        static char buffer[GDB_RX_BUF_LEN];

        char *hargs;

        uint32_t n;
        uint32_t r;
        uint32_t addr;
        uint32_t length;
        uint32_t kind;
        uint32_t type;
        char *data;

        int error;

        _gdb_last_signal(sigval);

        _gdb_monitor_entry(reg_file);

        while (true) {
                hargs = &*buffer;

                _get_packet(hargs);

                switch (*hargs++) {
                case '?':
                        /* '?'
                         * Indicate the reason the target halted */
                        _gdb_last_signal(sigval);
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

                        _put_packet('\0', "OK", 2);
                        return;
                case 'g':
                        /* 'g'
                         * Read general registers */
                        _gdb_command_read_registers(reg_file);
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
                        _put_packet('\0', "OK", 2);

                        _gdb_kill();
                        return;
                case 'm':
                        /* 'm addr,length'
                         * Read LENGTH bytes memory from specified ADDRess */
                        hargs = _parse_unsigned_long(hargs, &addr, ',');
                        _parse_unsigned_long(hargs, &length, '\0');

                        _gdb_command_read_memory(addr, length);
                        break;
                case 'M':
                        /* 'M addr,length:XX...'
                         * Write LENGTH bytes memory from ADDRess */

                        hargs = _parse_unsigned_long(hargs, &addr, ',');
                        data = _parse_unsigned_long(hargs, &length, ':');

                        _hex_buffer_to_mem(data, (void *)addr, length);

                        /* Send OK back to GDB */
                        _put_packet('\0', "OK", 2);
                        break;
                case 'P':
                        /* 'P n...=r...'
                         * Write to a specific register with a value */

                        hargs = _parse_unsigned_long(hargs, &n, '=');

                        _hex_buffer_to_mem(hargs, &r, sizeof(uint32_t));
                        _gdb_register_file_write(reg_file, n, r);

                        /* Send OK back to GDB */
                        _put_packet('\0', "OK", 2);
                        break;
                case 'R':
                        /* 'R XX'
                         * Restart the program being debugged */
                        break;
                case 's':
                        /* 's [addr]'
                         * Single step */
                        addr = 0x00000000;
                        _parse_unsigned_long(hargs, &addr, '\0');
                        _gdb_step(reg_file, addr);
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
                        hargs = _parse_unsigned_long(hargs, &type, ',');
                        hargs = _parse_unsigned_long(hargs, &addr, ',');
                        _parse_unsigned_long(hargs, &kind, '\0');

                        if ((error = _gdb_remove_break(type, addr, kind)) < 0) {
                                _put_packet('E', "01", 2);
                                break;
                        }

                        _put_packet('\0', "OK", 2);
                        break;
                case 'Z':
                        /* 'Z type,addr,kind'
                         * Insert breakpoint or watchpoint */

                        type = 0x00000000;
                        addr = 0x00000000;
                        hargs = _parse_unsigned_long(hargs, &type, ',');
                        hargs = _parse_unsigned_long(hargs, &addr, ',');
                        _parse_unsigned_long(hargs, &kind, '\0');

                        if ((error = _gdb_break(type, addr, kind)) < 0) {
                                _put_packet('E', "01", 2);
                                break;
                        }

                        /* Send OK back to GDB */
                        _put_packet('\0', "OK", 2);
                        break;
                default:
                        /* Invalid */
                        _put_packet('\0', NULL, 0);
                        break;
                }
        }
}

static void
_gdb_command_read_registers(cpu_registers_t *reg_file)
{
        char tx_buf[8];
        size_t tx_len;

        uint8_t csum;

        uint32_t n;
        uint32_t r;

        do {
                _gdb_putc('$');

                csum = 0;
                /* Loop through all registers */
                for (n = 0; _gdb_register_file_read(reg_file, n, &r); n++) {
                        tx_len = _mem_to_hex_buffer(&r, tx_buf, sizeof(r));
                        /* Send to GDB and calculate checksum */
                        csum += _put_buf(tx_buf, tx_len);
                }

                /* Send message footer */
                _gdb_putc('#');
                _gdb_putc(_low_nibble_to_hex(csum >> 4));
                _gdb_putc(_low_nibble_to_hex(csum));
        } while ((_gdb_getc() & 0x7F) != '+');
}

static void
_gdb_command_read_memory(uint32_t address, uint32_t len)
{
        char tx_buf[8];
        size_t tx_len;

        uint8_t csum;

        uint8_t *p;
        uint8_t r;
        uint32_t i;

        do {
                _gdb_putc('$');

                csum = 0x00;
                for (i = 0; i < len; i++) {
                        /* Read from memory */
                        p = (uint8_t *)(address + i);
                        r = *p;
                        tx_len = _mem_to_hex_buffer(&r, tx_buf, 1);

                        /* Send to GDB and calculate checksum */
                        csum += _put_buf(tx_buf, tx_len);
                }

                /* Send message footer */
                _gdb_putc('#');
                _gdb_putc(_low_nibble_to_hex(csum >> 4));
                _gdb_putc(_low_nibble_to_hex(csum));
        } while ((_gdb_getc() & 0x7F) != '+');
}

static void
_gdb_last_signal(int sigval)
{
        char tx_buf[2];

        tx_buf[0] = _low_nibble_to_hex(sigval >> 4);
        tx_buf[1] = _low_nibble_to_hex(sigval);

        /* Send packet */
        _put_packet('S', tx_buf, sizeof(tx_buf));
}

/* Packet format
 * $packet-data#checksum */
static void
_put_packet(char c, const char *buffer, size_t len)
{
        uint8_t csum;

        do {
                /* Send the header */
                _gdb_putc('$');

                /* Send the message type, if specified */
                if (c != '\0') {
                        _gdb_putc(c);
                }
                /* Send the data */
                csum = c + _put_buf(buffer, len);

                /* Send the footer */
                _gdb_putc('#');

                /* Checksum */
                _gdb_putc(_low_nibble_to_hex(csum >> 4));
                _gdb_putc(_low_nibble_to_hex(csum));
        } while ((_gdb_getc() & 0x7F) != '+');
}

/* Packet format
 * $<data>#<checksum> */
static void
_get_packet(char *rx_buffer)
{
        uint8_t ch;
        uint8_t csum;
        uint8_t xmit_csum;

        uint16_t len;
        uint16_t i;

        do {
                /* Wait around for start character, ignore all others */
                while (((ch = _gdb_getc()) & 0x7F) != '$') {
                        if (ch == '') {
                                /* ETX (end of text) */
                                ;
                        }
                }

                csum = 0x00;
                xmit_csum = 0xFF;

                /* Now, read until a # or end of buffer is found */
                for (len = 0; ; len++) {
                        ch = _gdb_getc() & 0x7F;
                        if (ch == '#') {
                                break;
                        }

                        csum += ch;
                        rx_buffer[len] = ch;
                }
                /* NULL terminate */
                rx_buffer[len] = '\0';

                /* Check if packet has a correct checksum */
                xmit_csum = _hex_digit_to_integer(_gdb_getc() & 0x7F);
                xmit_csum = _hex_digit_to_integer(_gdb_getc() & 0x7F) + (xmit_csum << 4);

                if (csum != xmit_csum) {
                        /* Bad checksum */
                        _gdb_putc('-');
                        continue;
                }
                /* Good checksum */
                _gdb_putc('+');

                /* If a sequence char is present, reply with the
                 * sequence ID */
                /* Packet format
                 * $sequence-id:packet-data#checksum */
                if (rx_buffer[2] == ':') {
                        /* XXX: Why is this here? */
                        assert(false);

                        _gdb_putc(rx_buffer[0]);
                        _gdb_putc(rx_buffer[1]);

                        /* Skip the first 3 characters by removing the
                         * sequence characters */
                        for (i = 3; i < len; i++) {
                                rx_buffer[i - 3] = rx_buffer[i];
                        }
                }
        } while (false);
}

/* Convert '[0-9a-fA-F]' to its integer equivalent */
static int
_hex_digit_to_integer(char h)
{
        if ((h >= 'a') && (h <= 'f')) {
                return h - 'a' + 10;
        }

        if ((h >= '0') && (h <= '9')) {
                return h - '0';
        }

        if ((h >= 'A') && (h <= 'F')) {
                return h - 'A' + 10;
        }

        return 0;
}

/* Convert the low nibble of C to its hexadecimal character
 * equivalent */
static char
_low_nibble_to_hex(char c)
{
        static const char lut[] = {
                "0123456789ABCDEF"
        };

        return lut[c & 0x0F];
}

/* Translates a delimited hexadecimal string to an unsigned long (32-bit) */
static char *_parse_unsigned_long(char *hargs, uint32_t *l, int delim)
{
        *l = 0x00000000;

        while (*hargs != delim) {
                *l = (*l << 4) + _hex_digit_to_integer(*hargs++);
        }

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
_mem_to_hex_buffer(const void *mem, char *h_buf, size_t len)
{
        lc_t lc_buf;

        uint32_t i;
        size_t cbuf_len;
        int ret_val;

        ret_val = 0;
        for (i = 0; len > 0; ) {
                if (IS_POW_2_ALIGNED(32, mem, len)) {
                        lc_buf.lbuf = *(uint32_t *)mem;
                        cbuf_len = sizeof(uint32_t);
                } else if (IS_POW_2_ALIGNED(16, mem, len)) {
                        lc_buf.sbuf = *(uint16_t *)mem;
                        cbuf_len = sizeof(uint16_t);
                } else {
                        lc_buf.cbuf[0] = *(char *)mem;
                        cbuf_len = sizeof(char);
                }

                mem = (void *)((uint32_t)mem + cbuf_len);
                len -= cbuf_len;

                for (i = 0; i < cbuf_len; i++) {
                        *h_buf++ = _low_nibble_to_hex(lc_buf.cbuf[i] >> 4);
                        *h_buf++ = _low_nibble_to_hex(lc_buf.cbuf[i]);
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
_hex_buffer_to_mem(const char *buf, void *mem, size_t len)
{
        lc_t lc_buf;

        size_t cbuf_len;
        uint32_t i;

        for (i = 0; len > 0; ) {
                if (IS_POW_2_ALIGNED(32, mem, len)) {
                        cbuf_len = sizeof(uint32_t);
                        for (i = 0; i < cbuf_len; i++) {
                                lc_buf.cbuf[i] = (_hex_digit_to_integer(*buf++) << 4);
                                lc_buf.cbuf[i] += _hex_digit_to_integer(*buf++);
                        }
                        *(uint32_t *)mem = lc_buf.lbuf;
                } else if (IS_POW_2_ALIGNED(16, mem, len)) {
                        cbuf_len = sizeof(uint16_t);
                        for (i = 0; i < cbuf_len; i++ ) {
                                lc_buf.cbuf[i] = (_hex_digit_to_integer(*buf++) << 4);
                                lc_buf.cbuf[i] += _hex_digit_to_integer(*buf++);
                        }
                        *(uint16_t *)mem = lc_buf.sbuf;
                } else {
                        lc_buf.cbuf[0] = (_hex_digit_to_integer(*buf++) << 4);
                        lc_buf.cbuf[0] += _hex_digit_to_integer(*buf++);

                        cbuf_len = sizeof(char);
                        *(char *)mem = lc_buf.cbuf[0];
                }
                mem = (void *)((uint32_t)mem + cbuf_len);
                len -= cbuf_len;
        }

        return mem;
}

static uint8_t
_put_buf(const char *buffer, int len)
{
        uint8_t sum;
        char c;

        sum = 0x00;
        while (len--) {
                c = *buffer++;
                sum += c;
                _gdb_putc(c);
        }

        return sum;
}
