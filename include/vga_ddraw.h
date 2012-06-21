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

//Filename    : vga_ddraw.h
//Description : Header file for class OVGA (Direct Draw version)

#ifndef __VGA_DDRAW_H
#define __VGA_DDRAW_H

#include <vga_base.h>
#include <windows.h>
#include <ddraw.h>
#include <asmfun.h>

//-------- Define class VgaDDraw ----------------//


struct RGBColor;
class ColorTable;

typedef struct HWND__ *HWND;	// vc6

class VgaDDraw : public VgaBase
{
public:

			union
			{
#ifdef __DDRAW_INCLUDED__
				LPDIRECTDRAW4		dd_obj;
#endif
				LPVOID				vptr_dd_obj;
			};

			union
			{
#ifdef __DDRAW_INCLUDED__
				LPDIRECTDRAWPALETTE	dd_pal;
#endif
				LPVOID					vptr_dd_pal;
			};

			union
			{
#ifdef __DDRAW_INCLUDED__
				PALETTEENTRY			pal_entry_buf[256];
#endif
				DWORD						dw_pal_entry_buf[256];
			};

		  ColorTable*				vga_color_table;
		  ColorTable*				vga_blend_table;
		  unsigned char			gray_remap_table[256];
		  int							pixel_format_flag;				// see IMGFUN.H	// new for 16-bit
		  short *					default_remap_table;				// new for 16-bit
		  short *					default_blend_table;				// new for 16-bit

		  HANDLE	app_hinstance;			// handle of the application running
		  HWND		main_hwnd;

public:
		  VgaDDraw();
		  ~VgaDDraw();

		  int    init();
		  int    set_mode(int width, int height);
		  void   deinit();

		  bool	is_inited();

		  void	adjust_brightness(int changeValue);

		  void	handle_messages() {};
		  void	flag_redraw() {};
		  int	is_full_screen() { return 0; }
		  void	toggle_full_screen() {};
		  void	flip();

		  int		translate_color(unsigned char c)			// calc 8-bit color to 16-bit color // new for 16 bit
					{ return default_remap_table[c]; }

		  int		make_pixel(BYTE red, BYTE green, BYTE blue);
		  int		make_pixel(RGBColor *);
		  void	decode_pixel(int, RGBColor *);

private:
		  int   init_dd();
		  void	init_gray_remap_table();
		  void	init_color_table();
		  int   init_pal(const char* fileName);
		  void  activate_pal(VgaBuf*);
		  void  release_pal();
		  int	create_window();
		  void	destroy_window();
};

typedef VgaDDraw Vga;
extern "C"
{
	extern short transparent_code_w __asmsym__("_transparent_code_w");
}

//--------------------------------------------//

#endif
