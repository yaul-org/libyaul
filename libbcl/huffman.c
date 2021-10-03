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

#include <sys/cdefs.h>

#include <stdint.h>

/* The maximum number of nodes in the Huffman tree is 2^(8+1)-1 = 511 */
#define MAX_TREE_NODES 511

typedef struct {
        uint8_t *byte_ptr;
        uint32_t bit_pos;
} huff_bitstream_t;

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

static void _bitstream_init(huff_bitstream_t *stream, uint8_t *buffer);
static uint32_t _bit_read(huff_bitstream_t *stream);
static uint32_t _8bits_read(huff_bitstream_t *stream);
static huff_decodenode_t *_tree_recover(huff_decodenode_t *nodes, huff_bitstream_t *stream, uint32_t *nodenum);

void
bcl_huffman_decompress(uint8_t *in, uint8_t *out, uint32_t in_size, uint32_t out_size)
{
        huff_decodenode_t nodes[MAX_TREE_NODES], *root, *node;
        huff_bitstream_t  stream;
        uint32_t k, node_count;
        uint8_t *buffer;

        /* Do we have anything to decompress? */
        if (in_size < 1) {
                return;
        }

        /* Initialize bitstream */
        _bitstream_init(&stream, in);

        /* Recover Huffman tree */
        node_count = 0;
        root = _tree_recover(nodes, &stream, &node_count);

        /* Decode input stream */
        buffer = out;

        for (k = 0; k < out_size; ++k) {
                /* Traverse tree until we find a matching leaf node */
                node = root;

                while (node->symbol < 0) {
                        /* Get next node */
                        if (_bit_read(&stream)) {
                                node = node->child_b;
                        } else {
                                node = node->child_a;
                        }
                }

                /* We found the matching leaf node and have the symbol */
                *buffer++ = (uint8_t)node->symbol;
        }
}

/* Initialize a bitstream */
static void
_bitstream_init(huff_bitstream_t *stream, uint8_t *buffer)
{
        stream->byte_ptr  = buffer;
        stream->bit_pos   = 0;
}

/* Read one bit from a bitstream */
static uint32_t
_bit_read(huff_bitstream_t *stream)
{
        uint32_t  x, bit;
        uint8_t *buffer;

        /* Get current stream state */
        buffer = stream->byte_ptr;
        bit = stream->bit_pos;

        /* Extract bit */
        x = (*buffer & (1 << (7 - bit))) ? 1 : 0;
        bit = (bit + 1) & 7;

        if (!bit) {
                ++buffer;
        }

        /* Store new stream state */
        stream->bit_pos = bit;
        stream->byte_ptr = buffer;

        return x;
}

/* Read eight bits from a bitstream */
static uint32_t
_8bits_read(huff_bitstream_t *stream)
{
        uint32_t  x, bit;
        uint8_t *buffer;

        /* Get current stream state */
        buffer = stream->byte_ptr;
        bit = stream->bit_pos;

        /* Extract byte */
        x = (*buffer << bit) | (buffer[1] >> (8 - bit));
        ++buffer;

        /* Store new stream state */
        stream->byte_ptr = buffer;

        return x;
}

/* Recover a Huffman tree from a bitstream */
static huff_decodenode_t *
_tree_recover(huff_decodenode_t *nodes, huff_bitstream_t *stream, uint32_t *nodenum)
{
        huff_decodenode_t *this_node;

        /* Pick a node from the node array */
        this_node = &nodes[*nodenum];
        *nodenum = *nodenum + 1;

        /* Clear the node */
        this_node->symbol = -1;
        this_node->child_a = NULL;
        this_node->child_b = NULL;

        /* Is this a leaf node? */
        if (_bit_read(stream)) {
                /* Get symbol from tree description and store in lead node */
                this_node->symbol = _8bits_read(stream);

                return this_node;
        }

        /* Get branch A */
        this_node->child_a = _tree_recover(nodes, stream, nodenum);

        /* Get branch B */
        this_node->child_b = _tree_recover(nodes, stream, nodenum);

        return this_node;
}
