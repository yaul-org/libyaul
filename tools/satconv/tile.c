/* tile.c: Converts tile BMP files to Saturn format
*  Copyright (C) 2021 Nathan Misner
*
*  This file is part of Satconv.
*
*  Satconv is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  Satconv is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with Satconv.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if _WIN32
	#include <winsock.h>
#else
	#include <arpa/inet.h>
#endif

#include "qdbmp.h"
#include "tile.h"

int tile_read(BMP *image, uint8_t *buffer, int width, int height, int start_x, int start_y) {
	uint8_t value;
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			if (((x + start_x) > width) || ((y + start_y) > height)) {
				printf("Error: Image width %d or height %d not a multiple of 8/16.\n", width, height);
				return 0;
			}
			BMP_GetPixelIndex(image, x + start_x, y + start_y, &value);
			buffer[y * 8 + x] = value;
		}
	}
	return 1;
}

void tile_writepalette(FILE *file, uint32_t *palette, int words, int len) {
    if (words == 2) {
        fwrite(palette, sizeof(uint32_t), len, file);
    }
    else {
        uint16_t *tempPalette = malloc(len * sizeof(uint16_t));
        uint32_t color;
        uint8_t r, g, b;
        for (int i = 0; i < len; i++) {
            color = ntohl(palette[i]);
            r = (color & 0xff) >> 3;
            g = ((color >> 8) & 0xff) >> 3;
            b = ((color >> 16) & 0xff) >> 3;
            tempPalette[i] = r | (g << 5) | (b << 10) | 0x8000;
            tempPalette[i] = htons(tempPalette[i]);
        }
        fwrite(tempPalette, sizeof(uint16_t), len, file);
        free(tempPalette);
    }

}

int tile_process(char *filename, char *outname, int type, int palwords) {
	BMP *infile = BMP_ReadFile(filename);
	BMP_CHECK_ERROR(stdout, 0);
	int width = BMP_GetWidth(infile);
	int height = BMP_GetHeight(infile);
	uint8_t bpp = BMP_GetDepth(infile);
	uint32_t palsize = 0;
	uint32_t *palette = NULL;
	int color;
	uint8_t r, g, b;

	if ((bpp == 4) || (bpp == 8)) {
		palsize = (1 << bpp);
		palette = malloc(palsize * sizeof(uint32_t));
		for (int i = 0; i < palsize; i++) {
			BMP_GetPaletteColor(infile, i, &r, &g, &b);
			color = r | (g << 8) | (b << 16);
			palette[i] = htonl(color); // saturn is big endian
		}
	}
	
	size_t image_data_size;
	if (bpp == 4) {
		image_data_size = (width >> 1) * height;
	}
	else if (bpp == 8) {
		image_data_size = width * height;
	}
	else {
		image_data_size = width * height * 2;
	}
	uint8_t *image_data = malloc(image_data_size);
	
	int image_cursor = 0;
	uint8_t single_tile[256];
	uint8_t val;
	switch (type) {
		case FRAMEBUFFER:
			// special case: output 512 color image
			if (bpp != 8) {
				// printf("Width: %d Height: %d\n", width, height);
				// yes i know this is retarded
				palette = malloc(512 * sizeof(uint32_t));
				for (int y = 0; y < height; y++) {
					for (int x = 0; x < width; x++) {
						BMP_GetPixelRGB(infile, x, y, &r, &g, &b);
						color = r | (g << 8) | (b << 16);
						// printf("%d\n", color);
						uint32_t out_color = htonl(color);
						int found = -1;
						for (int i = 0; i < palsize; i++) {
							if (palette[i] == out_color) {
								found = i;
								break;
							}
						}
						if (found == -1) {
							palette[palsize] = out_color;
							// saturn is big endian
							image_data[image_cursor++] = (palsize >> 8) & 0xff;
							image_data[image_cursor++] = palsize & 0xff;
							palsize++;
						}
						else {
							image_data[image_cursor++] = (found >> 8) & 0xff;
							image_data[image_cursor++] = found & 0xff;							
						}
					}
				}
			}
			else {
				for (int y = 0; y < height; y++) {
					for (int x = 0; x < width; x++) {
						BMP_GetPixelIndex(infile, x, y, &val);
						image_data[y * width + x] = val;
					}
				}
			}
			break;
			
		case TILE_8X8:
			if ((bpp != 4) && (bpp != 8)) {
				printf("Error: Only indexed images supported for tile mode.\n");
				free(image_data);
				BMP_Free(infile);
				return 0;
			}
			for (int y = 0; y < height; y += 8) {
				for (int x = 0; x < width; x += 8) {
					if (!tile_read(infile, single_tile, width, height, x, y)) {
						// error gets printed by tile_read
						goto ERR_EXIT;
					}
					if (bpp == 4) {
						for (int i = 0; i < 64; i += 2) {
							image_data[image_cursor++] = ((single_tile[i] & 0xF) << 4) | (single_tile[i + 1] & 0xF);
						}
					}
					else {
						for (int i = 0; i < 64; i++) {
							image_data[image_cursor++] = single_tile[i];
						}
					}
				}
			}
			break;
			
		case TILE_16x16:
			if ((bpp != 4) && (bpp != 8)) {
				printf("Error: Only indexed images supported for tile mode.\n");
				goto ERR_EXIT;
			}
			for (int y = 0; y < height; y += 16) {
				for (int x = 0; x < width; x += 16) {
					int res = 1;
					// top left
					res &= tile_read(infile, single_tile, width, height, x, y);
					// top right
					res &= tile_read(infile, single_tile + 64, width, height, x + 8, y);
					// bottom left
					res &= tile_read(infile, single_tile + 128, width, height, x, y + 8);
					// bottom right
					res &= tile_read(infile, single_tile + 192, width, height, x + 8, y + 8);
					if (!res) {
						goto ERR_EXIT;
					}
					if (bpp == 4) {
						for (int i = 0; i < 256; i += 2) {
							image_data[image_cursor++] = ((single_tile[i] & 0xF) << 4) | (single_tile[i + 1] & 0xF);
						}
					}
					else {
						for (int i = 0; i < 256; i++) {
							image_data[image_cursor++] = single_tile[i];
						}
					}
				}
			}
			break;
			
		default:
			printf("Error: invalid tile graphics type.\n");
			goto ERR_EXIT;
	}
		
	FILE *outfile = fopen(outname, "wb");
	if (!outfile) {
		printf("Error: Couldn't open %s for writing!\n", outname);
		goto ERR_EXIT;
	}
	uint32_t out_palsize = htonl(palsize);
	fwrite(&out_palsize, sizeof(uint32_t), 1, outfile);
    uint32_t out_palwords = htonl(palwords);
    fwrite(&out_palwords, sizeof(uint32_t), 1, outfile);
    tile_writepalette(outfile, palette, palwords, palsize);
	uint32_t out_image_data_size = htonl(image_data_size);
	fwrite(&out_image_data_size, sizeof(uint32_t), 1, outfile);
	fwrite(image_data, sizeof(uint8_t), image_data_size, outfile);
	fclose(outfile);
	
	free(palette);
	free(image_data);
	BMP_Free(infile);
	return 1;

ERR_EXIT:
	free(palette);
	free(image_data);
	BMP_Free(infile);
	return 0;
}
