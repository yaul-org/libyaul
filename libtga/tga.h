/*
 * Copyright (c) 2013, 2014-2016
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef TGA_H_
#define TGA_H_

#include <stdio.h>
#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define TGA_FILE_OK                     0
#define TGA_FILE_UNKNOWN_FORMAT         1
#define TGA_FILE_CORRUPTED              2
#define TGA_FILE_NOT_SUPPORTED          3
#define TGA_MEMORY_ERROR                4

typedef struct {
        const char *tga_file;

#define TGA_IMAGE_TYPE_NONE             0
#define TGA_IMAGE_TYPE_CMAP             1
#define TGA_IMAGE_TYPE_TRUE_COLOR       2
#define TGA_IMAGE_TYPE_GRAYSCALE        3
#define TGA_IMAGE_TYPE_RLE_CMAP         9
#define TGA_IMAGE_TYPE_RLE_TRUE_COLOR   10
#define TGA_IMAGE_TYPE_RLE_GRAYSCALE    11
        uint8_t tga_type;

        uint8_t tga_bpp;
        uint16_t tga_width;
        uint16_t tga_height;

        uint8_t tga_cmap_bpp;
        uint16_t tga_cmap_len;
        uint32_t tga_cmap_bytes;

        struct {
                /* Determine the transparent pixel color */
                uint32_t transparent_pixel;
                /* Enable setting MSB bit for transparent pixel color */
                bool msb;
        } tga_options;
} tga_t __attribute__ ((aligned(4)));

int tga_read(tga_t *, const uint8_t *);
uint32_t tga_image_decode_tiled(const tga_t *, void *);
uint32_t tga_image_decode(const tga_t *, void *);
uint32_t tga_cmap_decode(const tga_t *, uint16_t *);
const char *tga_error_stringify(int);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* TGA_H_ */
