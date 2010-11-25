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

// Filename    : OFLC.H
// Description : Header file of class FLC (animation format)


#ifndef __OFLC_H
#define __OFLC_H

class Flc
{
public:
	//TFAnimation*		flc_handle;
	unsigned char*		frame_buffer;
	unsigned char*		palette_buffer;
	int					init_flag;

public:
	Flc();
	~Flc();

	int	open_file(char *filename);
	int	open_mem(void *flcBuffer);
	void	deinit();

	static void set_error_handler( void (*handler)(char *msg) );

	// -------- get info function -------//
	short	width()   { return 0; }
	short	height()  { return 0; }
	short max_frame() { return 0; }
	short cur_frame() { return 0; }
	unsigned char *get_palette() { return 0; }

	int	advance();		// advance to next frame
	int	rewind();		// rewind to the beginning
	void	get_area(void *, short x1, short y1, short x2, short y2);
};
#endif
