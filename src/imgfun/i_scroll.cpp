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
 *Filename    : i_scroll.cpp
 *Description : scroll bitmap
 *
 * converted to c++
 */

//--------- BEGIN OF FUNCTION IMGscrollW -----------//
//
// Put an non-compressed bitmap on image buffer.
// It does not handle color key transparency.
//
// Syntax : IMGscrollW( imageBuf, pitch, srcX1, srcY1, srcX2, srcY2, destX1, destY1 )
//
// short *imageBuf - the pointer to the display surface buffer
// int pitch - the pitch of the display surface buffer
// int srcX1, srcY1, srcX2, srcY2 - window of scrolling
// int destX1, destY1 - destination window
//

void IMGscrollW(short *imageBuf, int pitch, int srcX1, int srcY1, int srcX2, int srcY2, int destX1, int destY1)
{
	int width = srcX2 - srcX1 + 1;
	int height = srcY2 - srcY1 + 1;
	int src = srcY1 * (pitch / 2) + srcX1;
	int dest = destY1 * (pitch / 2) + destX1;

	if (src > dest)
	{
		for (int j = 0; j < height; ++j, src += pitch / 2, dest += pitch / 2)
		{
			for (int i = 0; i < width; ++i)
			{
				imageBuf[dest + i] = imageBuf[src + i];
			}
		}
	}
	else if (src < dest)
	{
		src = srcY2 * (pitch / 2) + srcX2;
		dest = (destY1 + srcY2 - srcY1) * (pitch / 2) + (destX1 + srcX2 - srcX1);

		for (int j = 0; j < height; ++j, src -= pitch / 2, dest -= pitch / 2)
		{
			for (int i = 0; i < width; ++i)
			{
				imageBuf[dest - i] = imageBuf[src - i];
			}
		}
	}
}

//---------- END OF FUNCTION IMGscrollW ------------//
