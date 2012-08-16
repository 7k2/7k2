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
 *Filename    : i_read.cpp
 *Description : Read a bitmap from the display surface buffer
 *
 * converted to c++
 */

#include <cstring>

//----------- BEGIN OF FUNCTION IMGreadW ------------//
//
// Put an non-compressed bitmap on image buffer.
// It does not handle color key transparency.
//
// Syntax : IMGreadW( imageBuf, pitch, x1, y1, x2, y2, bitmapBuf )
//
// short *imageBuf     - the pointer to the display surface buffer
// int  pitch          - pitch of the surface buffer
// int  x1, y1, x2, y2 - the read of the surface buffer to read
// short *bitmapPtr    - the pointer to the bitmap buffer
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

void IMGreadW(short *imageBuf, int pitch, int x1, int y1, int x2, int y2, short *bitmapPtr)
{
	int width = x2-x1 + 1;
	int height = y2-y1 + 1;
	int offset;
	int src_offset;

	// Alex: note: this is a binary compatible replacement for the asm
	// TODO: convert bitmapPtr from char to a struct
	((unsigned char*)bitmapPtr)[0] = width & 0xff;
	((unsigned char*)bitmapPtr)[1] = width>>8;
	((unsigned char*)bitmapPtr)[2] = height & 0xff;
	((unsigned char*)bitmapPtr)[3] = height>>8;
	offset = 2;
	src_offset = y1 * (pitch/2) + x1;
	for (int y=0; y<height; ++y)
	{
		memcpy( &bitmapPtr[offset], &imageBuf[src_offset], width*2 );
		offset += width;
		src_offset += pitch/2;
	}
}

//----------- END OF FUNCTION IMGreadW ----------//
