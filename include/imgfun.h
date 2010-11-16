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
 */

// Filename    : IMGFUN.H
// Description : Declaration of assembly functions

#ifndef __IMGFUN_H
#define __IMGFUN_H

#include <asmfun.h>

// ----- define enum type PixelFormatFlag16 -------//

enum PixelFormatFlag
{
	PIXFORM_RGB_555 = 0,	// red on low bits
	PIXFORM_RGB_565 = 1,
	PIXFORM_BGR_555 = 2,	// blue on low bits
	PIXFORM_BGR_565 = 3,
};


extern "C"
{
	// basic functions

	void IMGcall INITeffect(int pixelFormatFlag) __asmsym__("_INITeffect");
	void IMGcall INITbright(int pixelFormatFlag) __asmsym__("_INITbright");
	void IMGcall IMGbar( short *imageBuf, int pitch, int x1, int y1, int x2, int y2, int color) __asmsym__("_IMGbar");
	void IMGcall IMGline( short *imageBuf, int pitch, int w, int h, int x1, int y1, int x2, int y2, int color) __asmsym__("_IMGline");
	void IMGcall IMGbarAlpha( short *imageBuf, int pitch, int x1, int y1, int x2, int y2, int logAlpha, int color) __asmsym__("_IMGbarAlpha");

	// 16-bit <-> 24 bit pixel conversion
	int IMGcall IMGmakePixel(int) __asmsym__("_IMGmakePixel");		// RGB_888 to pixel format specified by pixel_format_flag
	int IMGcall IMGdecodePixel(int) __asmsym__("_IMGdecodePixel");	// backward

	// 8->16 blt

	void IMGcall IMGbltRemap( short *imageBuf, int pitch, int x, int y, char *bitmapBuf,
		short *colorTable ) __asmsym__("_IMGbltRemap");
	void IMGcall IMGbltRemapHMirror( short *imageBuf, int pitch, int x, int y, char *bitmapBuf,
		short *colorTable ) __asmsym__("_IMGbltRemapHMirror");
	void IMGcall IMGbltAreaRemap( short *imageBuf, int pitch, int x, int y, char *bitmapBuf,
		int srcX1, int srcY1, int srcX2, int srcY2, short *colorTable ) __asmsym__("_IMGbltAreaRemap");
	void IMGcall IMGbltAreaRemapHMirror( short *imageBuf, int pitch, int x, int y, char *bitmapBuf,
		int srcX1, int srcY1, int srcX2, int srcY2, short *colorTable ) __asmsym__("_IMGbltAreaRemapHMirror");

	// functions with transparent color keying

	void IMGcall IMGbltTransRemap( short *imageBuf, int pitch, int x, int y, char *bitmapBuf,
		short *colorRemapTable ) __asmsym__("_IMGbltTransRemap");
	void IMGcall IMGbltTransRemapHMirror( short *imageBuf, int pitch, int x, int y, char *bitmapBuf,
		short *colorRemapTable ) __asmsym__("_IMGbltTransRemapHMirror");
	void IMGcall IMGbltAreaTransRemap( short *imageBuf, int pitch, int x, int y, char *bitmapBuf,
		int srcX1, int srcY1, int srcX2, int srcY2, short *colorTable ) __asmsym__("_IMGbltAreaTransRemap");
	void IMGcall IMGbltAreaTransRemapHMirror( short *imageBuf, int pitch, int x, int y, char *bitmapBuf,
		int srcX1, int srcY1, int srcX2, int srcY2, short *colorTable) __asmsym__("_IMGbltAreaTransRemapHMirror");

	// functions with run-length transparent key decompression

	void IMGcall IMGbltTransRemapDecompress( short *imageBuf, int pitch, int x, int y, char *bitmapBuf,
		short *colorTable) __asmsym__("_IMGbltTransRemapDecompress");
	void IMGcall IMGbltTransRemapDecompressHMirror( short *imageBuf, int pitch, int x, int y, char *bitmapBuf,
		short *colorTable) __asmsym__("_IMGbltTransRemapDecompressHMirror");
	void IMGcall IMGbltAreaTransRemapDecompress( short *imageBuf, int pitch, int x, int y, char *bitmapBuf,
		int srcX1, int srcY1, int srcX2, int srcY2, short *colorTable) __asmsym__("_IMGbltAreaTransRemapDecompress");
	void IMGcall IMGbltAreaTransRemapDecompressHMirror( short *imageBuf, int pitch, int x, int y, char *bitmapBuf,
		int srcX1, int srcY1, int srcX2, int srcY2, short *colorTable) __asmsym__("_IMGbltAreaTransRemapDecompressHMirror");

	// functions with run-length transparency key decompression, half-alpha

	void IMGcall IMGbltHalfRemapDecompress( short *imageBuf, int pitch, int x, int y, char *bitmapBuf,
		short *colorTable) __asmsym__("_IMGbltHalfRemapDecompress");
	void IMGcall IMGbltHalfRemapDecompressHMirror( short *imageBuf, int pitch, int x, int y, char *bitmapBuf,
		short *colorTable) __asmsym__("_IMGbltHalfRemapDecompressHMirror");
	void IMGcall IMGbltAreaHalfRemapDecompress( short *imageBuf, int pitch, int x, int y, char *bitmapBuf,
		int srcX1, int srcY1, int srcX2, int srcY2, short *colorTable) __asmsym__("_IMGbltAreaHalfRemapDecompress");
	void IMGcall IMGbltAreaHalfRemapDecompressHMirror( short *imageBuf, int pitch, int x, int y, char *bitmapBuf,
		int srcX1, int srcY1, int srcX2, int srcY2, short *colorTable) __asmsym__("_IMGbltAreaHalfRemapDecompressHMirror");

	// functions with blend bitmap

	void IMGcall IMGbltBlendRemap( short *imageBuf, int pitch, int x, int y, char *bitmapBuf,
		short *colorTable ) __asmsym__("_IMGbltBlendRemap");
	void IMGcall IMGbltBlendRemapHMirror( short *imageBuf, int pitch, int x, int y, char *bitmapBuf,
		short *colorTable ) __asmsym__("_IMGbltBlendRemapHMirror");
	void IMGcall IMGbltBlendAreaRemap( short *imageBuf, int pitch, int x, int y, char *bitmapBuf,
		int srcX1, int srcY1, int srcX2, int srcY2, short *colorTable ) __asmsym__("_IMGbltBlendAreaRemap");
	void IMGcall IMGbltBlendAreaRemapHMirror( short *imageBuf, int pitch, int x, int y, char *bitmapBuf,
		int srcX1, int srcY1, int srcX2, int srcY2, short *colorTable ) __asmsym__("_IMGbltBlendAreaRemapHMirror");

	// functions with weaker blend bitmap

	void IMGcall IMGbltWeakblendRemap( short *imageBuf, int pitch, int x, int y, char *bitmapBuf,
		short *colorTable ) __asmsym__("_IMGbltWeakblendRemap");
	void IMGcall IMGbltWeakblendRemapHMirror( short *imageBuf, int pitch, int x, int y, char *bitmapBuf,
		short *colorTable ) __asmsym__("_IMGbltWeakblendRemapHMirror");
	void IMGcall IMGbltWeakblendAreaRemap( short *imageBuf, int pitch, int x, int y, char *bitmapBuf,
		int srcX1, int srcY1, int srcX2, int srcY2, short *colorTable ) __asmsym__("_IMGbltWeakblendAreaRemap");
	void IMGcall IMGbltWeakblendAreaRemapHMirror( short *imageBuf, int pitch, int x, int y, char *bitmapBuf,
		int srcX1, int srcY1, int srcX2, int srcY2, short *colorTable ) __asmsym__("_IMGbltWeakblendAreaRemapHMirror");

	// 16->16 blt

	void IMGcall IMGreadW( short *imageBuf, int pitch, int x1, int y1, int x2, int y2, short *bitmapBuf ) __asmsym__("_IMGreadW");
	void IMGcall IMGbltW( short *imageBuf, int pitch, int x, int y, short *bitmapBuf ) __asmsym__("_IMGbltW");
	void IMGcall IMGbltWArea( short *imageBuf, int pitch, int x, int y, short *bitmapBuf,
		int srcX1, int srcY1, int srcX2, int srcY2 ) __asmsym__("_IMGbltWArea");
	void IMGcall IMGscrollW( short *imageBuf, int truePitch, int srcX1, int srcY1, int srcX2, int srcY2,
		int destX1, int destY1 ) __asmsym__("_IMGscrollW");	// see scroll_buffer() for complete usage

	// functions with transparent color keying

	void IMGcall IMGbltWTrans( short *imageBuf, int pitch, int x, int y, short *bitmapBuf ) __asmsym__("_IMGbltWTrans");
	void IMGcall IMGbltWAreaTrans( short *imageBuf, int pitch, int x, int y, short *bitmapBuf,
		int srcX1, int srcY1, int srcX2, int srcY2 ) __asmsym__("_IMGbltWAreaTrans");

	// ##### begin Gilbert 20/10 #######//
	// functions with drawing black mask 

	void IMGcall IMGbltWTransBlacken( short *imageBuf, int pitch, int x, int y, short *bitmapWBuf ) __asmsym__("_IMGbltWTransBlacken");
	void IMGcall IMGbltWAreaTransBlacken( short *imageBuf, int pitch, int x, int y, short *bitmapWBuf,
		int srcX1, int srcY1, int srcX2, int srcY2 ) __asmsym__("_IMGbltWAreaTransBlacken");
	// ##### end Gilbert 20/10 #######//

	// 16 bit buffer to 16 bit buffer blt

	void IMGcall IMGcopyW( short *imageBuf, int imgPitch, short *backBuf, int backPitch,
		int srcX1, int srcY1, int srcX2, int srcY2) __asmsym__("_IMGcopyW");
	void IMGcall IMGcopyWbright( short *imageBuf, int imgPitch, short *backBuf, int backPitch,
		int srcX1, int srcY1, int srcX2, int srcY2, int brightness) __asmsym__("_IMGcopyWbright");

	void IMGcall IMGbrightBar( short *imageBuf, int pitch, int x1, int y1, int x2, int y2, int brightness) __asmsym__("_IMGbrightBar");

	// join bitmap and back buffer to front buffer

	void IMGcall IMGjoinTransRemap( short *imageBuf, int imgPitch, short *backBuf, int backPitch,
		int x, int y, char *bitmapBuf, short *colorRemapTable ) __asmsym__("_IMGjoinTransRemap");
};

#endif
