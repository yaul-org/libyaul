#ifndef TGA_H_
#define TGA_H_

#include <stdio.h>
#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FILE_OK                 0
#define FILE_UNKNOWN_FORMAT     1
#define FILE_CORRUPTED          2
#define FILE_NOT_SUPPORTED      3

typedef struct {
#define TGA_IMAGE_TYPE_NONE             0x00
#define TGA_IMAGE_TYPE_CMAP             0x01
#define TGA_IMAGE_TYPE_TRUE_COLOR       0x02
#define TGA_IMAGE_TYPE_GRAYSCALE        0x03
#define TGA_IMAGE_TYPE_RLE_CMAP         0x09
#define TGA_IMAGE_TYPE_RLE_TRUE_COLOR   0x10
#define TGA_IMAGE_TYPE_RLE_GRAYSCALE    0x11
        uint8_t tga_type;
        uint8_t tga_bpp;
        uint16_t tga_width;
        uint16_t tga_height;

        uint8_t tga_cmap;
        uint8_t tga_cmapbpp;
        const uint16_t *tga_cmapbuf;
        uint8_t tga_cmaplen;

        const uint8_t *tga_imagebuf;
} tga_t;

#ifdef __cplusplus
}
#endif

#endif /* TGA_H_ */
