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
 *Filename    : ib.cpp
 *Description : Blt a bitmap to the display surface buffer without color key transparency handling
 *
 * converted to C++
 */

#include <cstring>

//----------- BEGIN OF FUNCTION IMGbltW ------------
//
// Put an non-compressed bitmap on image buffer.
// It does not handle color key transparency.
//
// Syntax : IMGbltW( imageBuf, pitch, x, y, bitmapBuf )
//
// short *imageBuf  - the pointer to the display surface buffer
// int   pitch      - pitch of the display surface buffer
// int   x,y        - where to put the image on the surface buffer
// short *bitmapBuf - the pointer to the bitmap buffer
//
//-------------------------------------------------
//
// Format of the bitmap data :
//
// <short>  width
// <short>  height
// <short..> bitmap image
//
//-------------------------------------------------

void IMGbltW(short *imageBuf, int pitch, int x, int y, short *bitmapBuf)
{
	int destline = y*(pitch/2) + x;
	int bitmapWidth = ((unsigned char*)bitmapBuf)[0] + (((unsigned char*)bitmapBuf)[1]<<8);
	int bitmapHeight = ((unsigned char*)bitmapBuf)[2] + (((unsigned char*)bitmapBuf)[3]<<8);
	int esi = 2;		// 4 bytes of bitmap header (16bit width, 16bit height)

	for ( int j=0; j<bitmapHeight; ++j,destline+=pitch/2,esi+=bitmapWidth )
	{
		memcpy( &imageBuf[destline], &bitmapBuf[esi], bitmapWidth*2 );
	}
}

//----------- END OF FUNCTION IMGbltW ----------
