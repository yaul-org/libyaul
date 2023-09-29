/* sprite.c: Converts sprite BMP files to Saturn format
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

#include <dirent.h>
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
#include "sprite.h"

#define  RGB16_COLOR(r,g,b) (((b)<<10) + ((g)<<5) + (r) + 0x8000)

#define PALETTE_SIZE (16)
static uint32_t **palette_list;
static int palette_cursor;

typedef struct {
	int x;
	int y;
	int pal;
	uint8_t *graphics;
} IMAGE_INFO;

static IMAGE_INFO *info_list;
static int info_cursor;

void Sprite_Convert4BPP(BMP *tile) {
	uint32_t palette_buffer[PALETTE_SIZE];
	uint8_t r, g, b;
	
	IMAGE_INFO *info = &info_list[info_cursor];
	info_cursor++;
	info->x = BMP_GetWidth(tile);
	info->y = BMP_GetHeight(tile);
	
	// read in the palette data
	int color;
	for (int i = 0; i < PALETTE_SIZE; i++) {
		BMP_GetPaletteColor(tile, i, &r, &g, &b);
		color = r | (g << 8) | (b << 16);
		palette_buffer[i] = htonl(color);
	}
	// check for a matching palette
	int palno = -1;
	for (int i = 0; i < palette_cursor; i++) {
		if (memcmp(palette_buffer, palette_list[i], PALETTE_SIZE * sizeof(uint32_t)) == 0) {
			palno = i;
			break;
		}
	}
	
	// add one if there isn't one
	if (palno == -1) {
		uint32_t *palette = palette_list[palette_cursor];
		memcpy(palette, palette_buffer, PALETTE_SIZE * sizeof(uint32_t));
		info->pal = palette_cursor;
		palette_cursor++;
	}
	else {
		info->pal = palno;
	}
	
	// read in the graphics data
	info->graphics = malloc((info->x >> 1) * info->y);
	uint8_t index1;
	uint8_t index2;
	uint8_t val;
	for (int y = 0; y < info->y; y++) {
		// 4bpp so we only count through half the x vals
		// and combine them together
		for (int x = 0; x < (info->x >> 1); x++) {
			BMP_GetPixelIndex(tile, x << 1, y, &index1);
			BMP_GetPixelIndex(tile, (x << 1) + 1, y, &index2);
			val = ((index1 & 0xF) << 4) | (index2 & 0xF);
			info->graphics[(y * (info->x >> 1)) + x] = val;
		}
	}
}

void Sprite_ConvertRGB(BMP *tile) {
	uint8_t r, g, b;
	uint16_t color;

	IMAGE_INFO *info = &info_list[info_cursor];
	info_cursor++;
	info->x = BMP_GetWidth(tile);
	info->y = BMP_GetHeight(tile);
	info->graphics = malloc(info->x * info->y * 2);

	int graphicsCursor = 0;
	for (int y = 0; y < info->y; y++) {
		for (int x = 0; x < info->x; x++) {
			BMP_GetPixelRGB(tile, x, y, &r, &g, &b);
			r >>= 3; g >>= 3; b >>= 3;
			color = RGB16_COLOR(r, g, b);
			color = htons(color);
			memcpy(&(info->graphics[graphicsCursor]), &color, sizeof(color));
			graphicsCursor += 2;
		}
	}
}

#define FILENAME_BUFLEN (256)

int sprite_process(char *dirname, char *outname, int type) {
	struct dirent *entry;
	DIR *dir_ptr;
	palette_cursor = 0;
	info_cursor = 0;
	char filename[FILENAME_BUFLEN];
	
	// get number of files in the directory
	int num_files = 0;
	dir_ptr = opendir(dirname);
	if (dir_ptr == NULL) {
		printf("Error: directory %s doesn't exist.\n", dirname);
		return 0;
	}
	while ((entry = readdir(dir_ptr)) != NULL) {
		if ((entry->d_name[0] == '.') || (strcmp(entry->d_name, "..") == 0)) {
			continue;
		}
		if (entry->d_type == DT_REG) {
			num_files++;
		}
	}
	closedir(dir_ptr);
	
	// allocate memory based on the number of files
	// the maximum number of palettes possible is one per frame
	if (type == SPRITE_4BPP) {
		palette_list = malloc(num_files * sizeof(uint32_t *));
		for (int i = 0; i < num_files; i++) {
			palette_list[i] = malloc(PALETTE_SIZE * sizeof(uint32_t));
		}
	}
	info_list = malloc(num_files * sizeof(IMAGE_INFO));
	
	// read in all the files
	for (int i = 0; i < num_files; i++) {
		int retval;
		if ((retval = snprintf(filename, FILENAME_BUFLEN, "%s/%d.bmp", dirname, i)) > FILENAME_BUFLEN) {
			printf("Error: overran filename buffer by %d bytes. Increase FILENAME_BUFLEN define in sprite.c.\n", retval);
			return 0;
		}
		printf("Reading %d %s\n", type, filename);
		BMP *tiledata = BMP_ReadFile(filename);
		BMP_CHECK_ERROR(stdout, 0);
		if (type == SPRITE_4BPP) {
			if (BMP_GetDepth(tiledata) != 4) {
				printf("Error: file %s isn't 4bpp.\n", entry->d_name);
				BMP_Free(tiledata);
				closedir(dir_ptr);
				return 0;
			}
			Sprite_Convert4BPP(tiledata);
		}
		else {
			Sprite_ConvertRGB(tiledata);
		}
		BMP_Free(tiledata);
	}
	// write out the sprite data
	FILE *out = fopen(outname, "wb");
	if (!out) {
		printf("Error: couldn't open %s for writing!\n", outname);
		for (int i = 0; i < num_files; i++) {
			free(palette_list[i]);
			free(info_list[i].graphics);
		}
		free(palette_list);
		free(info_list);
		return 0;
	}
	// saturn is big-endian so all the data over one byte must be byteswapped
	// write type
	int type_be = htonl(type);
	fwrite(&type_be, sizeof(type_be), 1, out);

	// write number of palettes
	if (type == SPRITE_4BPP) {
		int palette_cursor_be = htonl(palette_cursor);
		fwrite(&palette_cursor_be, sizeof(palette_cursor_be), 1, out);
	}
	// write all the palettes (they're already byteswapped)
	for (int i = 0; i < palette_cursor; i++) {
		printf("Writing palette %d\n", i);
		fwrite(palette_list[i], sizeof(uint32_t), PALETTE_SIZE, out);
	}
	// write number of sprites
	int num_files_be = htonl(num_files);
	fwrite(&num_files_be, sizeof(num_files_be), 1, out);
	// write IMAGE_INFO structs
	int tmp_x, tmp_y, tmp_pal;
	for (int i = 0; i < num_files; i++) {
		tmp_x = htonl(info_list[i].x);
		tmp_y = htonl(info_list[i].y);
		fwrite(&tmp_x, sizeof(tmp_x), 1, out);
		fwrite(&tmp_y, sizeof(tmp_y), 1, out);
		if (type == SPRITE_4BPP) {
			tmp_pal = htonl(info_list[i].pal);
			fwrite(&tmp_pal, sizeof(tmp_pal), 1, out);
			fwrite(info_list[i].graphics, sizeof(uint8_t), (info_list[i].x >> 1) * info_list[i].y, out);
		}
		else if (type == SPRITE_RGB) {
			fwrite(info_list[i].graphics, sizeof(uint8_t), info_list[i].x * info_list[i].y * 2, out);
		}
	}
	fclose(out);	
	// clean up memory
	for (int i = 0; i < num_files; i++) {
		if (type == SPRITE_4BPP) {
			free(palette_list[i]);
		}
		free(info_list[i].graphics);
	}
	free(palette_list);
	free(info_list);
	
	return 1;
}
