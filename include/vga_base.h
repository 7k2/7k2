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

//Filename    : vga_base.h
//Description : Defines the base vga class

#ifndef __VGA_BASE_H
#define __VGA_BASE_H

#include <ovgabuf.h>
#include <color.h>

//----------- define constants ----------//

#define MAX_BRIGHTNESS_ADJUST_DEGREE 32

//----------- Define constant -------------//

#define IF_LIGHT_BORDER_COLOR     V_WHITE
#define IF_DARK_BORDER_COLOR      V_BLACK
#define IF_UP_BRIGHTNESS_ADJUST   5
#define IF_DOWN_BRIGHTNESS_ADJUST 6

//-------- Define class VgaBase ----------------//

class VgaBase
{
public:
	static VgaBuf* active_buf;
	static char    use_back_buf;
	static char    opaque_flag;

public:
	VgaBase() {};
	~VgaBase() {};

	virtual int    init() =0;
	virtual void   deinit() =0;

	virtual bool   is_inited() =0;

	void use_front() { use_back_buf=0; active_buf = &vga_front; }
	void use_back()  { use_back_buf=1; active_buf = &vga_back;  }

	virtual void handle_messages() =0;
	virtual void flag_redraw() =0;
	virtual int is_full_screen() =0;
	virtual void set_full_screen_mode(int mode) =0;
	virtual void flip() =0;
	virtual int change_resolution(int width, int height) =0;
};

//--------------------------------------------//

#endif
