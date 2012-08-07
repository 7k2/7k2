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
 *Filename    : ib_ahrdm.cpp
 *Description : Blt a bitmap to the display surface buffer
 *              with decompression, transparency handling and colormaping
 *              and horizontal mirroring
 *
 * converted to C++
 */

#include <colcode.h>
#include <imgfun.h>

//----------- BEGIN OF FUNCTION IMGbltAreaHalfRemapDecompressHMirror ------
//
// Put a compressed bitmap on image buffer.
// It does handle color key transparency and remaping.
//
// Syntax : IMGbltAreaHalfRemapDecompressHMirror( imageBuf, pitch, x, y, bitmapBuf,
//                 x1, y1, x2, y2, colorTable)
//
// short *imageBuf   - the pointer to the display surface buffer
// int  x,y          - where to put the image on the surface buffer
// char *bitmapPtr   - the pointer to the bitmap buffer
// int  x1,y1,x2,y2  - clipping window from left, top of the bitmap
// short *colorTable - color remaping table
//
// the area to be blit to the screen is called clip window
// the width of clip window is stored in clipWindow
//
// ESI and AH points to a point in the bitmap data
// if ESI points to a compression key (i.e. F8-FF),
// AH refer to how many of points are passed.
// i.e. [esi] = F8 21, AH may to 0 to 20
//
// SeekForward function move the (ESI,AH) pointer
// to forward ECX points (decompressed)
//
// ESI and AH are positioned to the top right hand corner of clip window
// and move left, and then downward
// (EDI is increasing and EDI is decreasing on a line)
//
// After the cursor moved right, it checks for three cases:
// 1. non-transparent data
// 2. transparent data, but do not pass the left of the clip window
// 3. transparent data, and will pass the left of the clip window
//
// for case 1, blit the point and move one point left. If the left
// side of the clip window is passed, position EDI and (ESI,AH) to the
// right side of the clip window on next line.
//
// for case 2, simply move the EDI backward and (ESI,AH) forward by the
// run-length count
//
// for case 3, skip EDI and (ESI,AH) to the right side of the clip window
// on the next line
//
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

void IMGbltAreaHalfRemapDecompressHMirror(short *imageBuf, int pitch, int desX, int desY, char *bitmapBuf,
	int srcX1, int srcY1, int srcX2, int srcY2, short *colorTable)
{
	int destline = (desY+srcY1)*(pitch/2) + (desX+srcX1);
	int bitmapWidth = ((unsigned char*)bitmapBuf)[0] + (((unsigned char*)bitmapBuf)[1]<<8);
	int bitmapHeight = ((unsigned char*)bitmapBuf)[2] + (((unsigned char*)bitmapBuf)[3]<<8);
	int width = srcX2 - srcX1 + 1;
	int height = srcY2 - srcY1 + 1;
	int esi = 4;		// 4 bytes of bitmap header (16bit width, 16bit height)
	int pixelsToSkip;
	int al;
	int ax;
	int tmp_ax;

	if (height <= 0) return;

	SeekForward(&pixelsToSkip, &esi, bitmapBuf, (srcY1 +1) * bitmapWidth - srcX2 -1);

	for ( int j=0; j<height; ++j,destline+=pitch/2 )
	{
		for ( int i=width-1; i>=0; --i )	// NOTE: this is descending (mirrored)
		{
			if (pixelsToSkip != 0)
			{
				if (pixelsToSkip > i)
				{
					// skip to end of line
					pixelsToSkip -= i+1;
					break;
				}
				i -= pixelsToSkip;
				pixelsToSkip = 0;
			}
			al = ((unsigned char*)bitmapBuf)[ esi++ ];		// load source byte
			if (al < MIN_EFFECT_CODE)
			{
				ax = (colorTable[ al ] >> 1) & logAlphaMask[1];
				tmp_ax = imageBuf[ destline + i ];
				imageBuf[ destline + i ] = ax;
				ax = (tmp_ax >> 1) & logAlphaMask[1];
				imageBuf[ destline + i ] += ax;
			}
			else if (al < MANY_TRANSPARENT_CODE)
			{
				imageBuf[ destline + i ] = effect_proc(al, imageBuf[ destline + i ]);
			}
			else if (al == MANY_TRANSPARENT_CODE)
			{
				pixelsToSkip = ((unsigned char*)bitmapBuf)[ esi++ ] -1;		// skip many pixels
			}
			else
			{
				pixelsToSkip = 256 - al -1;					// skip (neg al) pixels
			}
		}
		// advance to next line
		if (j<height-1)		// but don't seek past end
		{
			pixelsToSkip -= bitmapWidth - width;
			if (pixelsToSkip < 0)
			{
				SeekForward(&pixelsToSkip, &esi, bitmapBuf, -pixelsToSkip);
			}
		}
	}
}

//----------- END OF FUNCTION IMGbltAreaHalfRemapDecompressHMirror ----
