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

// Filename    : OBITMAP.H
// Description : header file of bitmap class

#ifndef __OBITMAP_H
#define __OBITMAP_H

class Bitmap
{
public:
	short width_field;
	short height_field;
	unsigned char bitmap[1];		// indefinite size

private:
	Bitmap();				// cannot make any instant

public:
	void	init(short w, short h);

	short get_width() { return width_field; }
	short get_height() { return height_field; }
	short get_pitch();
	short get_true_pitch();
	unsigned char *get_ptr() { return bitmap; }
	unsigned char *get_ptr(short x, short y);

	long	size();

	static Bitmap *mem_add_bitmap(short w, short h);		// same parameter as init
	static long size(short w, short h);		// same parameter as init
};

#endif