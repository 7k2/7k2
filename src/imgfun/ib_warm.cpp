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
 *Filename    : ib_warm.cpp
 *Description : Blt a bitmap to the display surface buffer without color key transparency handling
 *              but with colour Remapping
 *
 * converted to C++
 */

#include <imgfun.h>

//-------- BEGIN OF FUNCTION IMGbltWeakblendAreaRemapHMirror ----------
//
// Put an non-compressed bitmap on image buffer.
// It does not handle color key transparency
// It can blt a specific area of the source image to the
// destination buffer instead of the whole source image.
// It handles colour remapping
//
// Syntax : IMGbltWeakblendAreaRemapHMirror( imageBuf, pitch, desX, desY, bitmapBuf, srcX1, srcY1, srcX2, srcY2, colorTable )
//
// short *imageBuf   - the pointer to the display surface buffer
// int  pitch        - pitch of the display surface buffer
// int  desX, desY   - where to put the area on the surface buffer
// char *bitmapPtr   - the pointer to the bitmap buffer
// int  srcX1, srcY1 - where to get the area on the source buffer
//      srcX2, srcY2
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

void IMGbltWeakblendAreaRemapHMirror(short *imageBuf, int pitch, int desX, int desY, char *bitmapPtr,
	int srcX1, int srcY1, int srcX2, int srcY2, short *colorTable)
{
	int destline = (desY+srcY1) * (pitch / 2) + (desX+srcX1);
	int bitmapWidth = ((unsigned char*)bitmapPtr)[0] + (((unsigned char*)bitmapPtr)[1]<<8);
	int srcline_end = 2 + srcY1 * bitmapWidth + srcX1 + bitmapWidth - 1;		// skip 4 byte header
	int width = srcX2-srcX1+1;
	int height = srcY2-srcY1+1;
	short ax;
	short dx;
	char ah;

	for ( int j=0; j<height; ++j, destline+=pitch/2, srcline_end+=bitmapWidth )
	{
		for ( int i=0; i<width; ++i )
		{
			ax = ((short*)bitmapPtr)[ srcline_end - i ];
			ah = (ax >> 9);
			ax = (ax & 0x00FF) | (ah << 8);
			dx = imageBuf[ destline + i ] >> ((ax & 0xFF00) >> 8);
			dx &= logAlphaMask[(ax & 0xFF00) >> 8];
			imageBuf[ destline + i ] = colorTable[ ax ] + dx;
		}
	}
}

//----------- END OF FUNCTION IMGbltWeakblendAreaRemapHMirror ----------
