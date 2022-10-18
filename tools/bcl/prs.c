/* Archive of the original code from fuzziqer. No license was provided with the
 * released files, although fuzziqer stated using the source freely was fine as
 * long as he's credited for the compression and decompression code. */

#include <inttypes.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shared.h"

#define PROGNAME "bcl_prs"

typedef struct {
        uint8_t bit_pos;
        uint8_t *control_byte_ptr;
        uint8_t *src_ptr_orig;
        uint8_t *dst_ptr_orig;
        uint8_t *src_ptr;
        uint8_t *dst_ptr;
} prs_compressor_t;

static uint32_t _prs_compress(void *in, void *dest, uint32_t size);

int
main(int argc, char *argv[])
{
        if (argc != 3) {
                print_usage(PROGNAME);

                return 0;
        }

        const char *const in_filename = argv[1];
        const char *const out_filename = argv[2];

        input_file_t input_file;

        if ((input_file_open(in_filename, &input_file)) != 0) {
                print_errno(PROGNAME);

                return 1;
        }

        /* Allocate a buffer of equal to twice the size since we don't know how
         * big the out file size will be */
        void *out_buffer;

        /* According to the original sources, the output buffer must be 0.4%
         * larger plus 1 byte */
        size_t out_size = (size_t)floor(1.004f * (float)input_file.buffer_len) + 1;

        if ((out_buffer = malloc(out_size)) == NULL) {
                print_errno(PROGNAME);

                return 1;
        }

        const uint32_t out_file_size =
                _prs_compress(input_file.buffer, out_buffer, input_file.buffer_len);

        (void)printf("%zu -> %"PRIu32"\n", input_file.buffer_len, out_file_size);

        if (out_file_size == 0) {
                fprintf(stderr, "Error: %s: RLE compression failed\n", PROGNAME);

                return 1;
        }

        if ((output_file_write(out_filename, out_buffer, out_file_size)) != 0) {
                print_errno(PROGNAME);

                return 1;
        }

        input_file_close(&input_file);

        free(out_buffer);

        return 0;
}

static void
_prs_put_control_bit(prs_compressor_t *pc, uint8_t bit)
{
        *pc->control_byte_ptr = *pc->control_byte_ptr >> 1;
        *pc->control_byte_ptr |= ((!!bit) << 7);
        pc->bit_pos++;

        if (pc->bit_pos >= 8) {
                pc->bit_pos = 0;
                pc->control_byte_ptr = pc->dst_ptr;
                pc->dst_ptr++;
        }
}

static void
_prs_put_control_bit_nosave(prs_compressor_t *pc, uint8_t bit)
{
        *pc->control_byte_ptr = *pc->control_byte_ptr >> 1;
        *pc->control_byte_ptr |= ((!!bit) << 7);
        pc->bit_pos++;
}

static void
_prs_put_control_save(prs_compressor_t *pc)
{
        if (pc->bit_pos >= 8) {
                pc->bit_pos = 0;
                pc->control_byte_ptr = pc->dst_ptr;
                pc->dst_ptr++;
        }
}

static void
_prs_put_static_data(prs_compressor_t *pc, uint8_t data)
{
        *pc->dst_ptr = data;
        pc->dst_ptr++;
}

static uint8_t
_prs_get_static_data(prs_compressor_t *pc)
{
        uint8_t data = *pc->src_ptr;
        pc->src_ptr++;
        return data;
}

static void
_prs_init(prs_compressor_t *pc, void *src, void *dst)
{
        pc->bit_pos = 0;
        pc->src_ptr = (uint8_t *)src;
        pc->src_ptr_orig = (uint8_t *)src;
        pc->dst_ptr = (uint8_t *)dst;
        pc->dst_ptr_orig = (uint8_t *)dst;
        pc->control_byte_ptr = pc->dst_ptr;
        pc->dst_ptr++;
}

static void
_prs_finish(prs_compressor_t *pc)
{
        _prs_put_control_bit(pc, 0);
        _prs_put_control_bit(pc, 1);

        if (pc->bit_pos != 0) {
                *pc->control_byte_ptr = ((*pc->control_byte_ptr << pc->bit_pos) >> 8);
        }

        _prs_put_static_data(pc, 0);
        _prs_put_static_data(pc, 0);
}

static void
_prs_rawbyte(prs_compressor_t *pc)
{
        _prs_put_control_bit_nosave(pc, 1);
        _prs_put_static_data(pc, _prs_get_static_data(pc));
        _prs_put_control_save(pc);
}

static void
_prs_shortcopy(prs_compressor_t *pc, int offset, uint8_t size)
{
        size -= 2;
        _prs_put_control_bit(pc, 0);
        _prs_put_control_bit(pc, 0);
        _prs_put_control_bit(pc, (size >> 1) & 1);
        _prs_put_control_bit_nosave(pc, size & 1);
        _prs_put_static_data(pc, offset & 0xFF);
        _prs_put_control_save(pc);
}

static void
_prs_longcopy(prs_compressor_t *pc, int offset, uint8_t size)
{
        uint8_t byte1, byte2;

        if (size <= 9) {
                _prs_put_control_bit(pc, 0);
                _prs_put_control_bit_nosave(pc, 1);
                _prs_put_static_data(pc, ((offset << 3) & 0xF8) | ((size - 2) & 0x07));
                _prs_put_static_data(pc, (offset >> 5) & 0xFF);
                _prs_put_control_save(pc);
        } else {
                _prs_put_control_bit(pc, 0);
                _prs_put_control_bit_nosave(pc, 1);
                _prs_put_static_data(pc, (offset << 3) & 0xF8);
                _prs_put_static_data(pc, (offset >> 5) & 0xFF);
                _prs_put_static_data(pc, size - 1);
                _prs_put_control_save(pc);
        }
}

static void
_prs_copy(prs_compressor_t *pc, int offset, uint8_t size)
{
        if ((offset > -0x100) && (size <= 5)) {
                _prs_shortcopy(pc, offset, size);
        } else {
                _prs_longcopy(pc, offset, size);
        }

        pc->src_ptr += size;
}

static uint32_t
_prs_compress(void *in, void *dest, uint32_t size)
{
        prs_compressor_t pc;
        int x, y, z;
        uint32_t xsize;
        int lsoffset, lssize;

        _prs_init(&pc, in, dest);

        for (x = 0; x < size; x++) {
                lsoffset = lssize = xsize = 0;

                for (y = x - 3; (y > 0) && (y > (x - 0x1FF0)) && (xsize < 255); y--) {
                        xsize = 3;

                        if (!memcmp((void *)((uintptr_t)in + y), (void *)((uintptr_t)in + x), xsize)) {
                                do {
                                        xsize++;
                                } while (!memcmp((void *)((uintptr_t)in + y),

                                                 (void *)((uintptr_t)in + x),
                                                 xsize) &&
                                                (xsize < 256) &&
                                                ((y + xsize) < x) &&
                                                ((x + xsize) <= size)
                                        );

                                xsize--;

                                if (xsize > lssize) {
                                        lsoffset = -(x - y);
                                        lssize = xsize;
                                }
                        }
                }

                if (lssize == 0) {
                        _prs_rawbyte(&pc);
                } else {
                        _prs_copy(&pc, lsoffset, lssize);
                        x += (lssize - 1);
                }
        }

        _prs_finish(&pc);

        return (pc.dst_ptr - pc.dst_ptr_orig);
}
