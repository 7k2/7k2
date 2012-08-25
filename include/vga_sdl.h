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

//Filename    : vga_sdl.h
//Description : Header file for class OVGA (SDL version)

#ifndef __VGA_SDL_H
#define __VGA_SDL_H

#include <vga_base.h>
#include "SDL.h"

//-------- Define class VgaSDL ----------------//


struct RGBColor;
class ColorTable;

class VgaSDL : public VgaBase
{
private:
	SDL_Window *window;
	SDL_Surface *screen;
	Uint32 video_mode_flags;
	SDL_Color game_pal[256];

public:
	ColorTable*	vga_color_table;
	ColorTable*	vga_blend_table;
	unsigned char	gray_remap_table[256];
	int		pixel_format_flag;				// see IMGFUN.H	// new for 16-bit
	short *		default_remap_table;				// new for 16-bit
	short *		default_blend_table;				// new for 16-bit

public:
	VgaSDL();
	~VgaSDL();

	int	init();
	int	set_mode(int width, int height);
	void	deinit();

	bool	is_inited();

	void	handle_messages();
	void	flag_redraw() {};
	int	is_full_screen() { return 0; }
	void	toggle_full_screen() {};
	void	flip();
	void	update_screen();

	int	translate_color(unsigned char c)			// calc 8-bit color to 16-bit color // new for 16 bit
			{ return default_remap_table[c]; }

	int	make_pixel(Uint8 red, Uint8 green, Uint8 blue);
	int	make_pixel(RGBColor *);
	void	decode_pixel(int, RGBColor *);

private:
	int   init_front(VgaBuf *);
	int   init_back(VgaBuf *, int width = 0, int height = 0);
	void  init_gray_remap_table();
	void  init_color_table();
	int   init_pal(const char* fileName);
	int   create_window();
	void  destroy_window();
};

typedef VgaSDL Vga;
extern short transparent_code_w;

//--------------------------------------------//

#endif
