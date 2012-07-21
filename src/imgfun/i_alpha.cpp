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
 *Filename    : i_alpha.cpp
 *Description : Draw a rectangle bar on vga image buffer alpha blending
 *
 * converted to C++
 */

#include <imgfun.h>

//----------- BEGIN OF FUNCTION IMGbarAlpha ------------
//
// Draw a bar on the VGA screen
//
// Note : No border checking is made in this function.
//        Placing an icon outside image buffer will cause serious BUG.
//
// short *imageBuf - the pointer to the display surface buffer
// int  pitch      - pitch of the display surface buffer
// int  x1,y1      - the top left vertex of the bar
// int  x2,y2      - the bottom right vertex of the bar
// int  logAlpha   - right shift bit count
// int  color      - the color of the line
//
//-------------------------------------------------
void IMGbarAlpha(short *imageBuf, int pitch, int x1, int y1, int x2, int y2, int logAlpha, int color)
{
	int destline = y1 * (pitch / 2) + x1;
	int width = x2 - x1 + 1;
	int height = y2 - y1 + 1;
	int mask = logAlphaMask[logAlpha];
	int value = color - ((color >> logAlpha) & mask);
	int ax = 0;

	for (int j = 0; j < height; ++j, destline += pitch / 2)
	{
		for (int i = 0; i < width; ++i)
		{
			ax = ((imageBuf[destline + i] >> logAlpha) & mask) + value;
			imageBuf[destline + i] = ax;
		}
	}
}
//----------- END OF FUNCTION IMGbarAlpha ----------
