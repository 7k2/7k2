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
 *
 *Filename    : i_bright.cpp
 *Description : Change brightness of a rectangle bar on vga image buffer
 *
 * converted to C++
 */

#include <stdint.h>

namespace
{

const int MAX_BRIGHTNESS_ADJUST_DEGREE = 32;
const int MAX_BRIGHTNESS_SHIFT = 5;
uint16_t red_table[2080] = {};
uint16_t green_table[2080] = {};
uint16_t blue_table[2080] = {};

}  // namespace


// --------- BEGIN OF FUNCTION INITbright -----------
//
// Initialization routine for IMGbrightBar
// call after the pixel format is determined
// Call once before any calling IMGbrightBar
//
// int pixelFormatFlag		pixel format of the primary surface
//

void INITbright(int pixelFormatFlag)
{
	int index = 0;
	int eax = 0;
	int edx = 0;

	for (int i = -32; i <= 32; ++i)
	{
		for (int j = 0; j < 32; ++j)
		{
			if (i <= 0)
			{
				edx = i + MAX_BRIGHTNESS_ADJUST_DEGREE;
				eax = (j * edx) >> MAX_BRIGHTNESS_SHIFT;
			}
			else
			{
				edx = MAX_BRIGHTNESS_ADJUST_DEGREE - i;
				eax = j * edx;
				edx = (i << 5) - i;
				eax = (eax + edx) >> MAX_BRIGHTNESS_SHIFT;
			}
			if (eax > 31)
			{
				eax = 31;
			}
			else if (eax < 0)
			{
				eax = 0;
			}
			blue_table[index] = eax;
			eax <<= 6;
			green_table[index] = eax;
			eax <<= 5;
			red_table[index] = eax;
			++index;
		}
	}
}

// --------- END OF FUNCTION INITbright -----------


// --------- BEGIN OF FUNCTION IMGbrightBar -----------
//
// Draw a bar on the VGA screen
//
// Note : No border checking is made in this function.
//        Placing an icon outside image buffer will cause serious BUG.
//
// short *imageBuf   - the pointer to the display surface buffer
// int  pitch        - the pitch of the display surface buffer
// int  x1,y1        - the top left vertex of the bar
// int  x2,y2        - the bottom right vertex of the bar
// int  brightness   - the brightness -32 to +32
//

void IMGbrightBar(short *imageBuf, int pitch, int x1, int y1, int x2, int y2, int brightness)
{
	int dest = y1 * (pitch / 2) + x1;
	int width = x2 - x1 + 1;
	int index = (brightness + MAX_BRIGHTNESS_ADJUST_DEGREE) << 5;
	uint16_t ax = 0;
	uint16_t bx = 0;
	uint16_t dx = 0;

	for (int y = y1; y <= y2; ++y, dest += pitch / 2)
	{
		for (int w = 0; w < width; ++w)
		{
			ax = ((uint16_t *)imageBuf)[dest + w];
			bx = ax;
			dx = ax;
			ax &= 0x1F;
			bx >>= 6;
			dx >>= 11;
			ax = blue_table[index + ax];
			bx &= 0x1F;
			dx &= 0x1F;
			ax |= green_table[index + bx];
			ax |= red_table[index + dx];
			((uint16_t *)imageBuf)[dest + w] = ax;
		}
	}
}

// ---------- END OF FUNCTION IMGbrightBar ------------
