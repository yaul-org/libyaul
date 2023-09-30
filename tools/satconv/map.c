/* map.c: Converts Tiled TMX maps to Saturn maps
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

#include "ezxml.h"
#include "map.h"

int map_readcsv(char **text) {
	char buff[16];
	int cursor = 0;
	while (**text && (**text != ',')) {
		buff[cursor++] = **text;
		(*text)++;
	}
	// move pointer past the comma
	if (**text) {
		(*text)++;
	}
	buff[cursor] = '\0';
	return atoi(buff);
}

// 1 word map
void map_conv1(char *instr, uint16_t *out, int bpp) {
	int in_val;
	uint16_t out_val;
	while (*instr) {
		// subtract 1 because tiled stores maps 1-indexed for some reason
		in_val = map_readcsv(&instr) - 1;
		if (bpp == 8) {
			out_val = (in_val & 0x3ff) << 1;
		}
		else if (bpp == 4) {
			out_val = (in_val & 0x3ff);
		}
		// is tile horizontally flipped?
		if (in_val & 0x80000000) {
			out_val |= 0x400;
		}
		// is tile vertically flipped?
		if (in_val & 0x40000000) {
			out_val |= 0x800;
		}
		*out = htons(out_val);
		out++;
	}
}

// 2 word map
// saturn 32-bit map format: YXPC00000CCCCCCC0AAAAAAAAAAAAAAA
void map_conv2(char *instr, uint32_t *out) {
	int in_val;
	uint32_t out_val;
	while (*instr) {
		in_val = map_readcsv(&instr) - 1;
		out_val = (in_val & 0x7fff) << 1;

		// is tile horizontally flipped?
		if (in_val & 0x80000000) {
			out_val |= 0x40000000;
		}
		// is tile vertically flipped?
		if (in_val & 0x80000000) {
			out_val |= 0x40000000;
		}
		// reverse endianness
		*out = htonl(out_val);
		out++;
	}
}

int map_process(char *inname, char *outname, int bpp, int type) {
	ezxml_t root_xml = ezxml_parse_file(inname);

	if ((bpp != 4) && (bpp != 8)) {
		printf("Error: invalid bpp %d\n", bpp);
		return 0;
	}

	if (root_xml == NULL) {
		printf("Error: couldn't open map file %s\n", inname);
		return 0;
	}

	// get attributes from the map
	const char *width_str = ezxml_attr(root_xml, "width");
	if (width_str == NULL) {
		printf("Error: couldn't find width attribute in map file %s\n", inname);
		ezxml_free(root_xml);
		return 0;
	}
	int width = atoi(width_str);
	int width_be = htonl(width);

	const char *height_str = ezxml_attr(root_xml, "height");
	if (height_str == NULL) {
		printf("Error: couldn't find height attribute in map file %s\n", inname);
		ezxml_free(root_xml);
		return 0;
	}
	int height = atoi(height_str);
	int height_be = htonl(height);

	// get map data (tiled stores it as csv)
	ezxml_t map = ezxml_get(root_xml, "layer", 0, "data", -1);
	if (map == NULL) {
		printf("Error: couldn't find map data in map file %s\n", inname);
		ezxml_free(root_xml);
		return 0;
	}

	if (type == 1) {
		uint16_t *map_data = malloc(width * height * sizeof(uint16_t));
		map_conv1(ezxml_txt(map), map_data, bpp);
		ezxml_free(root_xml);
		FILE *out_file = fopen(outname, "w");
		if (out_file == NULL) {
			printf("Error: couldn't open output file %s\n", outname);
			free(map_data);
			return 0;
		}
		fwrite(&width_be, sizeof(int), 1, out_file);
		fwrite(&height_be, sizeof(int), 1, out_file);
		fwrite(map_data, sizeof(uint16_t), width * height, out_file);
		free(map_data);
		fclose(out_file);
	}

	else if (type == 2) {
		uint32_t *map_data = malloc(width * height * sizeof(uint32_t));
		map_conv2(ezxml_txt(map), map_data);
		ezxml_free(root_xml);
		FILE *out_file = fopen(outname, "wb");
		if (out_file == NULL) {
			printf("Error: couldn't open output file %s\n", outname);
			free(map_data);
			return 0;
		}
		fwrite(&width_be, sizeof(int), 1, out_file);
		fwrite(&height_be, sizeof(int), 1, out_file);
		fwrite(map_data, sizeof(uint32_t), width * height, out_file);
		free(map_data);
		fclose(out_file);
	}

	else {
		printf("Error: invalid type %d\n", type);
		ezxml_free(root_xml);
		return 0;
	}

	return 1;
}
