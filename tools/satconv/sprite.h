/* sprite.h: Converts sprite BMP files to Saturn format
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

#ifndef SPRITE_H
#define SPRITE_H

typedef enum {
	SPRITE_4BPP = 0,
	SPRITE_RGB,
} SPRITE_TYPE;

// converts a directory of sprite graphics into one file.
// dirname: path to the graphics directory
// outfile: the file to write to
int sprite_process(char *dirname, char *outname, int type);

#endif
