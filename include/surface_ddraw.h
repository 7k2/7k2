/*
 * Seven Kingdoms 2: The Fryhtan War
 *
 * Copyright 1999 Enlight Software Ltd.
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

//Filename    : surface_ddraw.h
//Description : Header file for a Direct Draw Surface.

#ifndef __SURFACE_DDRAW_H
#define __SURFACE_DDRAW_H

#include <windows.h>
#include <ddraw.h>

//-------- Define class SurfaceDDraw ----------------//

class SurfaceDDraw
{
private:
	union
	{
#ifdef __DDRAW_INCLUDED__
		DDSURFACEDESC2	buf_des;
#endif
		char		c_buf_des[0x100];
	};

	int	buf_locked;			// whether the and back buffers have been locked or not.
	short*	cur_buf_ptr;
	long	cur_pitch;			// buf_des.lPitch

protected:
	union
	{
#ifdef __DDRAW_INCLUDED__
		LPDIRECTDRAWSURFACE4  dd_buf;
#endif
		LPVOID		      vptr_dd_buf;
	};

public:
	SurfaceDDraw(LPDIRECTDRAWSURFACE4 surface);
	~SurfaceDDraw();

	//---------- system functions ----------//

	bool	is_inited()		{ return vptr_dd_buf != NULL; }

	//--------- back buffer ----------//

	short*	buf_ptr()		{ return cur_buf_ptr; }
	short*	buf_ptr(int x, int y)	{ return (short *)((char *)cur_buf_ptr + cur_pitch*y) + x; }

	// pitch in pixel
	int 	buf_pitch()		{ return cur_pitch >> 1; }		// in no. of pixel

	// pitch in byte
	int 	buf_true_pitch()	{ return cur_pitch; }

	int	buf_size()		{ return buf_des.dwWidth * buf_des.dwHeight * sizeof(short); }
	int	buf_width()		{ return buf_des.dwWidth; }
	int	buf_height()		{ return buf_des.dwHeight; }

	BOOL	is_buf_lost();
	BOOL	restore_buf();

	int	lock_buf();
	int	unlock_buf();

	void	set_buf_ptr(short* bufPtr, long pitch)	{ cur_buf_ptr = bufPtr; cur_pitch = pitch; }
	void	set_default_buf_ptr()	{ cur_buf_ptr = (short*)buf_des.lpSurface; cur_pitch = buf_des.lPitch; }

	int 	write_bmp_file(char* fileName);

	// --------- VgaBuf to VgaBuf copy ------------ //

	void	blt_buf_area( SurfaceDDraw *srcBuf, int x1, int y1, int x2, int y2 );

	void	blt_virtual_buf( SurfaceDDraw *srcBuf );

	void	blt_virtual_buf_area( SurfaceDDraw *srcBuf, int x1, int y1, int x2, int y2 );
};

typedef SurfaceDDraw Surface;

//--------------------------------------------//

#endif // __SURFACE_DDRAW_H
