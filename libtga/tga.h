#ifndef TGA_H_
#define TGA_H_

#include <stdio.h>
#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TGA_FILE_OK                     0
#define TGA_FILE_UNKNOWN_FORMAT         1
#define TGA_FILE_CORRUPTED              2
#define TGA_FILE_NOT_SUPPORTED          3
#define TGA_MEMORY_ERROR                4

typedef struct {
#define TGA_IMAGE_TYPE_NONE             0
#define TGA_IMAGE_TYPE_CMAP             1
#define TGA_IMAGE_TYPE_TRUE_COLOR       2
#define TGA_IMAGE_TYPE_GRAYSCALE        3
#define TGA_IMAGE_TYPE_RLE_CMAP         9
#define TGA_IMAGE_TYPE_RLE_TRUE_COLOR   10
#define TGA_IMAGE_TYPE_RLE_GRAYSCALE    11
        uint8_t tga_type;

        uint8_t tga_cmapbpp;
        uint8_t tga_cmaplen;

        uint8_t tga_bpp;
        uint16_t tga_width;
        uint16_t tga_height;
} tga_t;

int tga_load(tga_t *, const uint8_t *, uint16_t *, uint16_t *);

#ifdef __cplusplus
}
#endif

#endif /* TGA_H_ */
