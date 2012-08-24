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
 *Filename    : ij_tr.cpp
 *Description : Add a bitmap and another display surface to the display
 *              surface on the same place with transparency handling
 *
 * converted to C++
 */

#include <colcode.h>

//-------- BEGIN OF FUNCTION IMGjoinTransRemap ----------
//
// Put an non-compressed bitmap on image buffer.
// It handles color key transparency. The color key code is 255.
//
// Syntax : IMGjoinTransRemap( imageBuf, imgPitch, backBuf, backPitch, x, y, bitmapBuf, colorRemapTable )
//
// short *imageBuf        - the pointer to the display surface buffer
// int imgPitch           - the pitch of the display surface buffer
// short *backBuf         - the pointer to the back buffer
// int backPitch          - the pitch of the back buffer
// int  x,y               - where to put the image on the surface buffer
// char *bitmapPtr        - the pointer to the bitmap buffer
// short *colorRemapTable - color remap table
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

void IMGjoinTransRemap(short *imageBuf, int imgPitch, short *backBuf, int backPitch,
	int x, int y, char *bitmapPtr, short *colorRemapTable)
{
	int destline = y * (imgPitch / 2) + x;
	int backline = y * (backPitch / 2) + x;
	int width = ((unsigned char*)bitmapPtr)[0] + (((unsigned char*)bitmapPtr)[1]<<8);
	int height = ((unsigned char*)bitmapPtr)[2] + (((unsigned char*)bitmapPtr)[3]<<8);
	int srcline = 4;		// skip 4 byte header
	int al;

	for ( int j=0; j<height; ++j, destline+=imgPitch/2, backline+=backPitch/2, srcline+=width )
	{
		for ( int i=0; i<width; ++i )
		{
			al = ((unsigned char*)bitmapPtr)[ srcline + i ];
			if (al == TRANSPARENT_CODE)
			{
				imageBuf[ destline + i ] = backBuf[ backline + i ];
			}
			else
			{
				imageBuf[ destline + i ] = colorRemapTable[ al ];
			}
		}
	}
}

//----------- END OF FUNCTION IMGjoinTransRemap ----------
