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
 *Filename    : ib_tr.cpp
 *Description : Blt a bitmap to the display surface buffer with colour remapping
 *              but with color key transparency handling
 *
 * converted to C++
 */

#include <colcode.h>

//----------- BEGIN OF FUNCTION IMGbltTransRemap ------------
//
// Put an non-compressed bitmap on image buffer.
// It does not handle color key transparency but colorRemapping
//
// Syntax : IMBbltTransRemap( imageBuf, pitch, x, y, bitmapBuf, colorRemapTable )
//
// short *imageBuf   - the pointer to the display surface buffer
// int  pitch        - pitch of the display surface buffer
// int  x,y          - where to put the image on the surface buffer
// char *bitmapPtr   - the pointer to the bitmap buffer
// short *colorTable - a 256-entry color remapping table
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

void IMGbltTransRemap(short *imageBuf, int pitch, int x, int y, char *bitmapBuf, short *colorTable)
{
	int destline = y*(pitch/2) + x;
	int bitmapWidth = ((unsigned char*)bitmapBuf)[0] + (((unsigned char*)bitmapBuf)[1]<<8);
	int bitmapHeight = ((unsigned char*)bitmapBuf)[2] + (((unsigned char*)bitmapBuf)[3]<<8);
	int esi = 4;	// 4 bytes are header fields (width, height)

	for ( int j=0; j<bitmapHeight; ++j,destline+=pitch/2,esi+=bitmapWidth )
	{
		for ( int i=0; i<bitmapWidth; ++i )
		{
			if ( ((unsigned char*)bitmapBuf)[esi + i] != TRANSPARENT_CODE)
			{
				imageBuf[destline + i] = colorTable[ ((unsigned char*)bitmapBuf)[esi+i] ];
			}
		}
	}
}

//----------- END OF FUNCTION IMGbltTransRemap ----------
