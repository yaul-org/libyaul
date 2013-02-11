/*
 * Copyright (c) 2013
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 * David Oberhollenzer
 */

#include <string.h>
#include <stdlib.h>

#include "tga.h"

#define READ_LITTLE_ENDIAN_16(array, index)                                    \
        (((size_t)(array)[(index)]) | ((size_t)(array)[(index) + 1]) << 8)

#define READ_BIG_ENDIAN_16(array, index)                                       \
        (((size_t)(array)[(index) + 1]) | (((size_t)(array)[(index)]) << 8))

#define TGA_HEADER_LEN  18
#define TGA_FOOTER_LEN  26

#define TGA_PACKET_TYPE_RLE 0
#define TGA_PACKET_TYPE_RAW 1

static uint16_t *tga_cmap_rgb555_decode(const uint8_t *, size_t, uint8_t);
static uint8_t *tga_rle_image_decode(const uint8_t *, uint16_t, uint16_t,
    uint8_t);

int
tga_load(tga_t *tga, const uint8_t *file)
{
        const uint8_t *header;
        const uint8_t *image_buf;
        const uint16_t *cmap_buf;

        uint8_t id_len;
        uint8_t image_type;
        uint8_t bytespp;
        uint16_t width;
        uint16_t height;

        uint8_t have_cmap;
        uint8_t cmap_bpp;
        uint16_t cmap_offset;
        uint16_t cmap_len;
        size_t cmap_bytes;

        /* Read the TGA header */
        header = &file[0];
        id_len = header[0];

        /* Skip the header */
        file += TGA_HEADER_LEN;

        image_type = header[2];
        bytespp = header[16] >> 3;
        width = READ_LITTLE_ENDIAN_16(header, 12);
        height = READ_LITTLE_ENDIAN_16(header, 14);

        have_cmap = header[1];
        cmap_buf = NULL;
        cmap_offset = READ_LITTLE_ENDIAN_16(header, 3);
        cmap_len = READ_LITTLE_ENDIAN_16(header, 5);
        cmap_bpp = header[7];
        cmap_bytes = cmap_len * (cmap_bpp >> 3);

        /* Sanity checks */
        if ((width == 0) || (height == 0))
                return FILE_CORRUPTED;

        switch (image_type) {
        case TGA_IMAGE_TYPE_GRAYSCALE:
        case TGA_IMAGE_TYPE_RLE_GRAYSCALE:
                if ((bytespp != 1) || have_cmap)
                        return FILE_CORRUPTED;
                break;
        case TGA_IMAGE_TYPE_TRUE_COLOR:
        case TGA_IMAGE_TYPE_RLE_TRUE_COLOR:
                if (((bytespp != 3) && (bytespp != 4)) || have_cmap)
                        return FILE_CORRUPTED;
                break;
        case TGA_IMAGE_TYPE_CMAP:
        case TGA_IMAGE_TYPE_RLE_CMAP:
                if (!have_cmap || (cmap_offset >= cmap_len))
                        return FILE_CORRUPTED;
                break;
        case TGA_IMAGE_TYPE_NONE:
        default:
                return FILE_CORRUPTED;
        }

        file += id_len;

        /* Read the color map if present and allocate an image buffer */
        if (have_cmap) {
                cmap_buf = tga_cmap_rgb555_decode(file, cmap_len, cmap_bpp);
                if (cmap_buf == NULL)
                        /* XXX: What should we return here? malloc() error? */
                        return FILE_OK;

                file += cmap_bytes;
        }

        image_buf = file;

        switch (image_type) {
        case TGA_IMAGE_TYPE_GRAYSCALE:
                break;
        case TGA_IMAGE_TYPE_RLE_GRAYSCALE:
                return FILE_NOT_SUPPORTED;
        case TGA_IMAGE_TYPE_TRUE_COLOR:
                break;
        case TGA_IMAGE_TYPE_RLE_TRUE_COLOR:
                return FILE_NOT_SUPPORTED;
        case TGA_IMAGE_TYPE_CMAP:
                break;
        case TGA_IMAGE_TYPE_RLE_CMAP:
                tga_rle_image_decode(image_buf, width, height, bytespp);
                break;
        case TGA_IMAGE_TYPE_NONE:
        default:
                return FILE_CORRUPTED;
        }

        tga->tga_type = image_type;
        tga->tga_bpp = bytespp << 3;
        tga->tga_width = width;
        tga->tga_height = height;

        tga->tga_cmap = have_cmap;
        tga->tga_cmapbpp = cmap_bpp;
        tga->tga_cmapbuf = cmap_buf;
        tga->tga_cmaplen = cmap_len;

        tga->tga_imagebuf = image_buf;

        return FILE_OK;
}

static uint16_t *
tga_cmap_rgb555_decode(const uint8_t *cmap_buf, size_t cmap_len, uint8_t cmap_bpp)
{
        uint16_t *cmap555_buf;
        uint16_t cmap_idx;

        cmap555_buf = (uint16_t *)malloc(cmap_len * sizeof(uint16_t));
        if (cmap555_buf == NULL)
                return NULL;

        uint8_t r;
        uint8_t g;
        uint8_t b;

        if (cmap_bpp == 8)
                return NULL;
        else if (cmap_bpp == 16) {
                for (cmap_idx = 0; cmap_idx < cmap_len; cmap_idx++) {
                        uint16_t color;

                        color = *(uint16_t *)&cmap_buf[cmap_idx * 2];
                        r = (color & 0x7C00) >> 8;
                        g = color & 0x03E0;
                        b = color & 0x001F;

                        /* Swap channels R and B */
                        cmap555_buf[cmap_idx] = (b << 10) | (g << 5) | r;
                }

                return cmap555_buf;
        } else if (cmap_bpp == 24) {
                for (cmap_idx = 0; cmap_idx < cmap_len; cmap_idx++) {
                        r = cmap_buf[(cmap_idx * 3) + 2] >> 3;
                        g = cmap_buf[(cmap_idx * 3) + 1] >> 3;
                        b = cmap_buf[cmap_idx * 3] >> 3;

                        cmap555_buf[cmap_idx] = (b << 10) | (g << 5) | r;
                }

                return cmap555_buf;
        } else if (cmap_bpp == 32) {
                for (cmap_idx = 0; cmap_idx < cmap_len; cmap_idx++) {
                        r = cmap_buf[(cmap_idx * 4) + 2] >> 3;
                        g = cmap_buf[(cmap_idx * 4) + 1] >> 3;
                        b = cmap_buf[cmap_idx * 4] >> 3;

                        cmap555_buf[cmap_idx] = (b << 10) | (g << 5) | r;
                }

                return cmap555_buf;
        }

        return NULL;
}

static uint8_t *
tga_rle_image_decode(const uint8_t *image_buf, uint16_t width, uint16_t height,
    uint8_t bytespp)
{
        const uint8_t *packet;
        const uint8_t *next_packet;

        uint8_t *uncompd_buf;
        uint8_t *uncompd_ptr;

        uint8_t rcf;
        uint8_t rcf_cnt;
        uint8_t packet_type;

        size_t image_len;

        image_len = width * height * bytespp;

        uncompd_buf = (uint8_t *)malloc(width * height * bytespp);
        if (uncompd_buf == NULL)
                return NULL;

        packet = image_buf;
        uncompd_ptr = uncompd_buf;

        for (; image_len > 0; image_len -= rcf) {
                packet_type = (packet[0] & 0x80) == 0;
                rcf = (packet[0] & ~0x80) + 1;

                switch (packet_type) {
                case TGA_PACKET_TYPE_RAW:
                        next_packet = (uint8_t *)(packet + (rcf * bytespp) + 1);
                        break;
                case TGA_PACKET_TYPE_RLE:
                        next_packet = (uint8_t *)(packet + bytespp + 1);
                        break;
                }

                for (rcf_cnt = 0; rcf_cnt < rcf; rcf_cnt++) {
                        uint8_t inc;

                        inc = (packet_type == TGA_PACKET_TYPE_RAW)
                            ? rcf_cnt
                            : 0;

                        switch (bytespp) {
                        case 1:
                                *uncompd_ptr++ = packet[1 + inc];
                                break;
                        case 2:
                                *uncompd_ptr++ = packet[1 + inc];
                                *uncompd_ptr++ = packet[1 + (inc + 1)];
                                break;
                        }
                }

                packet = next_packet;
        }

        return uncompd_buf;
}
