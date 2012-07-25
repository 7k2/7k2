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
 *Filename    : ib_r.cpp
 *Description : Blt a bitmap to the display surface buffer with colour remapping
 *              but without color key transparency handling
 *
 * converted to C++
 */

//-------- BEGIN OF FUNCTION IMGbltRemap ----------
//
// Put an non-compressed bitmap on image buffer.
// It does not handle color key transparency but colorRemapping
//
// Syntax : IMGbltRemap( imageBuf, pitch, x, y, bitmapBuf, colorTable )
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

void IMGbltRemap(short *imageBuf, int pitch, int x, int y, char *bitmapPtr, short *colorTable)
{
	int destline = y * (pitch / 2) + x;
	int width = ((unsigned char*)bitmapPtr)[0] + (((unsigned char*)bitmapPtr)[1]<<8);
	int height = ((unsigned char*)bitmapPtr)[2] + (((unsigned char*)bitmapPtr)[3]<<8);
	int srcline = 4;		// skip 4 byte header
	int ax;

	for ( int j=0; j<height; ++j, destline+=pitch/2, srcline+=width )
	{
		for ( int i=0; i<width; ++i )
		{
			ax = ((unsigned char*)bitmapPtr)[ srcline + i ];
			imageBuf[ destline + i ] = colorTable[ ax ];
		}
	}
}

//----------- END OF FUNCTION IMGbltRemap ----------
