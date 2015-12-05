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

//Filename    : vga_none.h
//Description : Header file for class OVGA (Empty version)

#ifndef __VGA_NONE_H
#define __VGA_NONE_H

#include <vga_base.h>

//-------- Define class VgaNone ----------------//


struct RGBColor;
class ColorTable;

class VgaNone : public VgaBase
{
public:
		  ColorTable*				vga_color_table;
		  ColorTable*				vga_blend_table;
		  unsigned char			gray_remap_table[256];
		  int							pixel_format_flag;				// see IMGFUN.H	// new for 16-bit
		  short *					default_remap_table;				// new for 16-bit
		  short *					default_blend_table;				// new for 16-bit

public:
		  VgaNone() {};
		  ~VgaNone() {};

		  int    init() { return 1; }
		  int    set_mode(int width, int height) { return 1; }
		  void   deinit() {};

		  bool	is_inited() { return false; }

		  void	adjust_brightness(int changeValue) {};

		  void	handle_messages() {};
		  void	flag_redraw() {};
		  int	is_full_screen() { return 0; }
		  void	set_full_screen_mode(int mode) {};
		  void	flip() {};

		  int		translate_color(unsigned char c)			// calc 8-bit color to 16-bit color // new for 16 bit
					{ return default_remap_table[c]; }

		  int		make_pixel(BYTE red, BYTE green, BYTE blue) { return 0; }
		  int		make_pixel(RGBColor *) { return 0; }
		  void	decode_pixel(int, RGBColor *) {};
};

typedef VgaNone Vga;
extern short transparent_code_w;

//--------------------------------------------//

#endif
