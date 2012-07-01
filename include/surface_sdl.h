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

//Filename    : surface_sdl.h
//Description : Header file for an SDL Surface.

#ifndef __SURFACE_SDL_H
#define __SURFACE_SDL_H

#include <cstddef>
#include "SDL.h"
#include <win32_compat.h>

class SurfaceSDL
{
private:
	SDL_Surface *surface;
	void *save_buf;
	long cur_pitch;

public:
	SurfaceSDL(SDL_Surface *s);
	~SurfaceSDL();

	bool	is_inited()		{ return surface != NULL; }

	short*	buf_ptr()		{ return (short *)surface->pixels; }
	short*	buf_ptr(int x, int y)	{ return (short *)((char *)surface->pixels + cur_pitch*y) + x; }

	// pitch in pixel
	int 	buf_pitch()		{ return cur_pitch >> 1; }		// in no. of pixel

	// pitch in byte
	int 	buf_true_pitch()	{ return cur_pitch; }

	int	buf_size()		{ return surface->w * surface->h * sizeof(short); }
	int	buf_width()		{ return surface->w; }
	int	buf_height()		{ return surface->h; }

	BOOL	is_buf_lost();
	BOOL	restore_buf();

	int	lock_buf();
	int	unlock_buf();

	void	set_buf_ptr(short* bufPtr, long pitch)	{ if (!save_buf) { save_buf = surface->pixels; surface->pixels = bufPtr; cur_pitch = pitch; } }
	void	set_default_buf_ptr()	{ if (save_buf) { surface->pixels = save_buf; save_buf = NULL; cur_pitch = surface->pitch; } }

	int 	write_bmp_file(char* fileName);

	void	blt_buf_area(SurfaceSDL *srcBuf, int x1, int y1, int x2, int y2);
	void	blt_virtual_buf(SurfaceSDL *srcBuf);
	void	blt_virtual_buf_area(SurfaceSDL *srcBuf, int x1, int y1, int x2, int y2);
	SDL_Surface *get_surface();
};

typedef SurfaceSDL Surface;

//--------------------------------------------//

#endif // __SURFACE_SDL_H
