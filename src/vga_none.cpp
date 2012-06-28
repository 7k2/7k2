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

//Filename    : vga_none.cpp
//Description : VGA manipulation functions (No output)

#include <ovga.h>

//------ Define static class member vars ---------//

char    VgaBase::use_back_buf = 0;
char    VgaBase::opaque_flag  = 0;
VgaBuf* VgaBase::active_buf   = &vga_front;      // default: front buffer

char    low_video_memory_flag = 0;
extern "C"
{
	short transparent_code_w;
}
