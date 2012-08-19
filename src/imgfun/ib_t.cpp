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
 *Filename    : ib_t.cpp
 *Description : Blt a bitmap to the display surface buffer with color key transparency handling
 *
 * converted to C++
 */

#include <colcode.h>

//-------- BEGIN OF FUNCTION IMGbltWTrans ----------
//
// Put an non-compressed bitmap on image buffer.
// It handles color key transparency. The color key code is 255.
//
// Syntax : IMGbltWTrans( imageBuf, pitch, x, y, bitmapBuf )
//
// short *imageBuf  - the pointer to the display surface buffer
// int  pitch       - pitch of the display surface buffer
// int  x,y         - where to put the image on the surface buffer
// short *bitmapPtr - the pointer to the bitmap buffer
//
//-------------------------------------------------
//
// Format of the bitmap data :
//
// <short>  width
// <short>  height
// <char..> bitmap image
//
//-------------------------------------------------

void IMGbltWTrans(short *imageBuf, int pitch, int x, int y, short *bitmapPtr)
{
	int destline = y * (pitch/2) + x;
	int width = ((unsigned char*)bitmapPtr)[0] + (((unsigned char*)bitmapPtr)[1]<<8);
	int height = ((unsigned char*)bitmapPtr)[2] + (((unsigned char*)bitmapPtr)[3]<<8);
	int srcline = 2;		// skip 4 byte header
	short bx;

	for ( int j=0; j<height; ++j, destline+=pitch/2, srcline+=width )
	{
		for ( int i=0; i<width; ++i )
		{
			bx = ((unsigned short*)bitmapPtr)[ srcline + i ];
			if (bx != transparent_code_w)
			{
				imageBuf[ destline + i ] = bx;
			}
		}
	}
}

//----------- END OF FUNCTION IMGbltWTrans ----------
