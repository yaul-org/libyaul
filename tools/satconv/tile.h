/* tile.h: Converts tile BMP files to Saturn format
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

#ifndef TILE_H
#define TILE_H

typedef enum {
	FRAMEBUFFER = 0,
	TILE_8X8,
	TILE_16x16,
} TILE_TYPE;

int tile_process(char *filename, char *outname, int type, int palwords);

#endif
