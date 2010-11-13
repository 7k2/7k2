/*
 * Seven Kingdoms 2: The Fryhtan War
 *
 * Copyright 1999 Enlight Software Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

// Filename    : OJPEG.H
// Description : ojpeg decompress class

#ifndef __OJPEG_H
#define __OJPEG_H

// problem with jpeglib.h : FILE not defined c/cpp includes jpeglib.h includes stdio.h first

class VgaBuf;
struct jpeg_error_mgr;
struct jpeg_decompress_struct;


class Jpeg
{
private:
	jpeg_decompress_struct* cinfo;
	jpeg_error_mgr*         jerr;
	int	error_flag;

public:
	Jpeg();
	~Jpeg();

	int put_to_buf( VgaBuf *vgaBuf, int x, int y, char *filename );
};

extern Jpeg jpeg;

#endif