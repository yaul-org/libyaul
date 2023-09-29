/* satconv.c: Entry point for program
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

#include <stdio.h>
#include <string.h>

#include "map.h"
#include "sprite.h"
#include "tile.h"

#define MIN(x, y) ((x) < (y) ? (x) : (y))

#define BUFFER_LEN (256)
#define ASCII_NUMBER_BASE (0x30)

int main(int argc, char **argv) {
	if (argc < 2) {
		printf("Usage: satconv [asset list]\n");
		return -1;
	}
	
	FILE *asset_list = fopen(argv[1], "r");
	if (asset_list == NULL) {
		printf("Error: couldn't open asset list.\n");
		return -2;
	}
	
	char line[BUFFER_LEN];
	while (fgets(line, BUFFER_LEN - 1, asset_list)) {
		line[strcspn(line, "\r\n")] = 0;
		char *infile;
		char *ext;
		int ext_index;
		int type;
		#define OUTFILE_LEN (32)
		char outfile[OUTFILE_LEN];

		switch(line[0]) {
			// sprite directory
			case 's':
			case 'S':
				switch(line[1]) {
					case '4':
						type = SPRITE_4BPP;
						break;

					case 'r':
					case 'R':
						type = SPRITE_RGB;
						break;

					default:
						printf("Invalid sprite type %c\nBad line: %s\n", line[1], line);
						continue;
				}

				infile = &line[3];
				// cd-rom filesystem only allows 8.3 filenames
				ext_index = MIN(8, strlen(infile));
				memcpy(outfile, infile, ext_index);
				strcpy(outfile + ext_index, ".spr");
				sprite_process(infile, outfile, type);
				break;
			
			// tile graphics
			case 't':
			case 'T':
				infile = &line[4];
				ext = strchr(infile, '.');
				ext_index = MIN(8, ext - infile);
				memcpy(outfile, infile, ext_index);
				strcpy(outfile + ext_index, ".tle");
				tile_process(infile, outfile, line[1] - ASCII_NUMBER_BASE, line[2] - ASCII_NUMBER_BASE);
				break;
			
			// tilemap
			case 'm':
			case 'M':
				infile = &line[4];
				ext = strchr(infile, '.');
				ext_index = MIN(8, ext - infile);
				memcpy(outfile, infile, ext_index);
				strcpy(outfile + ext_index, ".map");
				map_process(infile, outfile, line[1] - ASCII_NUMBER_BASE, line[2] - ASCII_NUMBER_BASE);
				break;
			
			default:
				break;
				
		}
	}
	
	fclose(asset_list);
	return 0;
}
