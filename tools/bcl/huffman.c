/* Copyright (c) 2003-2006 Marcus Geelnard
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not claim
 *    that you wrote the original software. If you use this software in a
 *    product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution. */

#include <errno.h>
#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "shared.h"

#define PROGNAME "bcl_huffman"

/* The maximum number of nodes in the Huffman tree is 2^(8+1)-1 = 511 */
#define MAX_TREE_NODES 511

typedef struct {
        uint8_t *byte_ptr;
        uint32_t bit_pos;
} huff_bitstream_t;

typedef struct {
        int32_t symbol;
        uint32_t count;
        uint32_t code;
        uint32_t bits;
} huff_sym_t;

typedef struct huff_encodenode huff_encodenode_t;

struct huff_encodenode {
        huff_encodenode_t *child_a, *child_b;
        int32_t count;
        int32_t symbol;
};

typedef struct huff_decodenode huff_decodenode_t;

struct huff_decodenode {
        huff_decodenode_t *child_a, *child_b;
        int32_t symbol;
};

int _huffman_compress(uint8_t *in, uint8_t *out, uint32_t in_size);

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
                _huffman_compress(input_file.buffer, out_buffer, input_file.buffer_len);

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

/* Initialize a bitstream */
static void _huffman_bitstream_init(huff_bitstream_t *stream, uint8_t *buf)
{
        stream->byte_ptr  = buf;
        stream->bit_pos   = 0;
}

/* Write bits to a bitstream */
static void
_huffman_bits_write(huff_bitstream_t *stream, uint32_t x, uint32_t bits)
{
        uint32_t  bit, count;
        uint8_t *buf;
        uint32_t  mask;

        /* Get current stream state */
        buf = stream->byte_ptr;
        bit = stream->bit_pos;

        /* Append bits */
        mask = 1 << (bits - 1);

        for (count = 0; count < bits; ++count) {
                *buf = (*buf & (0xff ^ (1 << (7 - bit)))) +
                       ((x & mask ? 1 : 0) << (7 - bit));
                x <<= 1;
                bit = (bit + 1) & 7;

                if (!bit) {
                        ++buf;
                }
        }

        /* Store new stream state */
        stream->byte_ptr = buf;
        stream->bit_pos  = bit;
}

/* Calculate (sorted) histogram for a block of data */
static void
_huffman_histogram_calculate(uint8_t *in, huff_sym_t *sym, uint32_t size)
{
        int k;

        /* Clear/init histogram */
        for (k = 0; k < 256; ++k) {
                sym[k].symbol = k;
                sym[k].count  = 0;
                sym[k].code   = 0;
                sym[k].bits   = 0;
        }

        /* Build histogram */
        for (k = size; k; --k) {
                sym[*in ++].count ++;
        }
}

/* Store a Huffman tree in the output stream and in a look-up-table (a symbol
 * array) */
static void
_huffman_tree_store(huff_encodenode_t *node, huff_sym_t *sym, huff_bitstream_t *stream, uint32_t code, uint32_t bits)
{
        uint32_t sym_idx;

        /* Is this a leaf node? */
        if (node->symbol >= 0) {
                /* Append symbol to tree description */
                _huffman_bits_write(stream, 1, 1);
                _huffman_bits_write(stream, node->symbol, 8);

                /* Find symbol index */
                for (sym_idx = 0; sym_idx < 256; ++sym_idx) {
                        if (sym[sym_idx].symbol == node->symbol) {
                                break;
                        }
                }

                /* Store code info in symbol array */
                sym[sym_idx].code = code;
                sym[sym_idx].bits = bits;
                return;
        } else {
                /* This was not a leaf node */
                _huffman_bits_write(stream, 0, 1);
        }

        /* Branch A */
        _huffman_tree_store(node->child_a, sym, stream, (code << 1) + 0, bits + 1);

        /* Branch B */
        _huffman_tree_store(node->child_b, sym, stream, (code << 1) + 1, bits + 1);
}

/* Generate a Huffman tree */
static void
_huffman_tree_make(huff_sym_t *sym, huff_bitstream_t *stream)
{
        huff_encodenode_t nodes[MAX_TREE_NODES], *node_1, *node_2, *root;
        uint32_t k, num_symbols, nodes_left, next_idx;

        /* Initialize all leaf nodes */
        num_symbols = 0;

        for (k = 0; k < 256; ++k) {
                if (sym[k].count > 0) {
                        nodes[num_symbols].symbol = sym[k].symbol;
                        nodes[num_symbols].count = sym[k].count;
                        nodes[num_symbols].child_a = NULL;
                        nodes[num_symbols].child_b = NULL;
                        ++num_symbols;
                }
        }

        /* Build tree by joining the lightest nodes until there is only
           one node left (the root node). */
        root = NULL;
        nodes_left = num_symbols;
        next_idx = num_symbols;

        while (nodes_left > 1) {
                /* Find the two lightest nodes */
                node_1 = NULL;
                node_2 = NULL;

                for (k = 0; k < next_idx; ++k) {
                        if (nodes[k].count > 0) {
                                if (!node_1 || (nodes[k].count <= node_1->count)) {
                                        node_2 = node_1;
                                        node_1 = &nodes[k];
                                } else if (!node_2 || (nodes[k].count <= node_2->count)) {
                                        node_2 = &nodes[k];
                                }
                        }
                }

                /* Join the two nodes into a new parent node */
                root = &nodes[next_idx];
                root->child_a = node_1;
                root->child_b = node_2;
                root->count = node_1->count + node_2->count;
                root->symbol = -1;
                node_1->count = 0;
                node_2->count = 0;
                ++next_idx;
                --nodes_left;
        }

        /* Store the tree in the output stream, and in the sym[] array (the
           latter is used as a look-up-table for faster encoding) */
        if (root) {
                _huffman_tree_store(root, sym, stream, 0, 0);
        } else {
                /* Special case: only one symbol => no binary tree */
                root = &nodes[0];
                _huffman_tree_store(root, sym, stream, 0, 1);
        }
}

int
_huffman_compress(uint8_t *in, uint8_t *out, uint32_t in_size)
{
        huff_sym_t sym[256], tmp;
        huff_bitstream_t stream;
        uint32_t k, total_bytes, swaps, symbol;

        /* Do we have anything to compress? */
        if (in_size < 1) {
                return 0;
        }

        /* Initialize bitstream */
        _huffman_bitstream_init(&stream, out);

        /* Calculate and sort histogram for input data */
        _huffman_histogram_calculate(in, sym, in_size);

        /* Build Huffman tree */
        _huffman_tree_make(sym, &stream);

        /* Sort histogram - first symbol first (bubble sort) */
        do {
                swaps = 0;

                for (k = 0; k < 255; ++k) {
                        if (sym[k].symbol > sym[k + 1].symbol) {
                                tmp        = sym[k];
                                sym[k]     = sym[k + 1];
                                sym[k + 1] = tmp;
                                swaps      = 1;
                        }
                }
        } while (swaps);

        /* Encode input stream */
        for (k = 0; k < in_size; ++k) {
                symbol = in[k];
                _huffman_bits_write(&stream, sym[symbol].code,
                                    sym[symbol].bits);
        }

        /* Calculate size of output data */
        total_bytes = (int)(stream.byte_ptr - out);

        if (stream.bit_pos > 0) {
                ++total_bytes;
        }

        return total_bytes;
}
