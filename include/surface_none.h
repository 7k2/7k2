/*
 * Seven Kingdoms 2: The Fryhtan War
 *
 * Copyright 2010 Jesse Allen
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

//Filename    : surface_none.h
//Description : Header file for dummy surfaces.

#ifndef __SURFACE_NONE_H
#define __SURFACE_NONE_H

//-------- Define class SurfaceNone ----------------//

class SurfaceNone
{
public:
	SurfaceNone() { }
	~SurfaceNone() { }

	//---------- system functions ----------//

	bool	is_inited()		{ return false; }

	//--------- back buffer ----------//

	short*	buf_ptr()		{ return NULL; }
	short*	buf_ptr(int x, int y)	{ return NULL; }

	// pitch in pixel
	int 	buf_pitch()		{ return 0; }		// in no. of pixel

	// pitch in byte
	int 	buf_true_pitch()	{ return 0; }

	int	buf_size()		{ return 0; }
	int	buf_width()		{ return 0; }
	int	buf_height()		{ return 0; }

	int	is_buf_lost()		{ return 0; }
	int	restore_buf()		{ return 1; }

	int	lock_buf()		{ return 0; }
	int	unlock_buf()		{ return 0; }

	void	set_buf_ptr(short* bufPtr, long pitch)	{ }
	void	set_default_buf_ptr()	{ }

	int 	write_bmp_file(char* fileName) { return 1; }

	// --------- VgaBuf to VgaBuf copy ------------ //

	void	blt_buf_area( SurfaceNone *srcBuf, int x1, int y1, int x2, int y2 ) { }

	void	blt_virtual_buf( SurfaceNone *srcBuf ) { }

	void	blt_virtual_buf_area( SurfaceNone *srcBuf, int x1, int y1, int x2, int y2 ) { }
};

typedef SurfaceNone Surface;

//--------------------------------------------//

#endif // __SURFACE_NONE_H
