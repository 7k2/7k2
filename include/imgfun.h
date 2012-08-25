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

#include <stdint.h>

// ----- define enum type PixelFormatFlag16 -------//

enum PixelFormatFlag
{
	PIXFORM_RGB_555 = 0,	// red on low bits
	PIXFORM_RGB_565 = 1,
	PIXFORM_BGR_555 = 2,	// blue on low bits
	PIXFORM_BGR_565 = 3,
};

static uint16_t logAlphaMask[8] =
{
	0xFFFF,
	0x7BEF,
	0x39E7,
	0x18E3,
	0x861,
	0x20,
	0,
	0
};

static const int MIN_EFFECT_CODE = 0xEF;

uint32_t effect_proc( int effect_id, uint16_t dest_pixel );

void SeekForward( /*out*/ int *ppixelsToSkip, /*inout*/ int *pesi, char *bitmapBuf, int seekAmount );

// basic functions

void INITbright(int pixelFormatFlag);
void IMGbar( short *imageBuf, int pitch, int x1, int y1, int x2, int y2, int color );
void IMGline( short *imageBuf, int pitch, int w, int h, int x1, int y1, int x2, int y2, int color);
void IMGbarAlpha( short *imageBuf, int pitch, int x1, int y1, int x2, int y2, int logAlpha, int color);

// 8->16 blt

void IMGbltRemap( short *imageBuf, int pitch, int x, int y, char *bitmapBuf,
	short *colorTable );
inline void IMGbltRemapHMirror( short *imageBuf, int pitch, int x, int y, char *bitmapBuf,
	short *colorTable ) { /* unused */ }
void IMGbltAreaRemap( short *imageBuf, int pitch, int x, int y, char *bitmapBuf,
	int srcX1, int srcY1, int srcX2, int srcY2, short *colorTable );
inline void IMGbltAreaRemapHMirror( short *imageBuf, int pitch, int x, int y, char *bitmapBuf,
	int srcX1, int srcY1, int srcX2, int srcY2, short *colorTable ) { /* unused */ }

// functions with transparent color keying

void IMGbltTransRemap( short *imageBuf, int pitch, int x, int y, char *bitmapBuf,
	short *colorRemapTable );
void IMGbltTransRemapHMirror( short *imageBuf, int pitch, int x, int y, char *bitmapBuf,
	short *colorRemapTable );
void IMGbltAreaTransRemap( short *imageBuf, int pitch, int x, int y, char *bitmapBuf,
	int srcX1, int srcY1, int srcX2, int srcY2, short *colorTable );
void IMGbltAreaTransRemapHMirror( short *imageBuf, int pitch, int x, int y, char *bitmapBuf,
	int srcX1, int srcY1, int srcX2, int srcY2, short *colorTable);

// functions with run-length transparent key decompression

void IMGbltTransRemapDecompress( short *imageBuf, int pitch, int x, int y, char *bitmapBuf,
	short *colorTable);
void IMGbltTransRemapDecompressHMirror( short *imageBuf, int pitch, int x, int y, char *bitmapBuf,
	short *colorTable);
void IMGbltAreaTransRemapDecompress( short *imageBuf, int pitch, int x, int y, char *bitmapBuf,
	int srcX1, int srcY1, int srcX2, int srcY2, short *colorTable);
void IMGbltAreaTransRemapDecompressHMirror( short *imageBuf, int pitch, int x, int y, char *bitmapBuf,
	int srcX1, int srcY1, int srcX2, int srcY2, short *colorTable);

// functions with run-length transparency key decompression, half-alpha

void IMGbltHalfRemapDecompress( short *imageBuf, int pitch, int x, int y, char *bitmapBuf,
	short *colorTable);
void IMGbltHalfRemapDecompressHMirror( short *imageBuf, int pitch, int x, int y, char *bitmapBuf,
	short *colorTable);
void IMGbltAreaHalfRemapDecompress( short *imageBuf, int pitch, int x, int y, char *bitmapBuf,
	int srcX1, int srcY1, int srcX2, int srcY2, short *colorTable);
void IMGbltAreaHalfRemapDecompressHMirror( short *imageBuf, int pitch, int x, int y, char *bitmapBuf,
	int srcX1, int srcY1, int srcX2, int srcY2, short *colorTable);

// functions with blend bitmap

void IMGbltBlendRemap( short *imageBuf, int pitch, int x, int y, char *bitmapBuf,
	short *colorTable );
void IMGbltBlendRemapHMirror( short *imageBuf, int pitch, int x, int y, char *bitmapBuf,
	short *colorTable );
void IMGbltBlendAreaRemap( short *imageBuf, int pitch, int x, int y, char *bitmapBuf,
	int srcX1, int srcY1, int srcX2, int srcY2, short *colorTable );
void IMGbltBlendAreaRemapHMirror( short *imageBuf, int pitch, int x, int y, char *bitmapBuf,
	int srcX1, int srcY1, int srcX2, int srcY2, short *colorTable );

// functions with weaker blend bitmap

void IMGbltWeakblendRemap( short *imageBuf, int pitch, int x, int y, char *bitmapBuf,
	short *colorTable );
void IMGbltWeakblendRemapHMirror( short *imageBuf, int pitch, int x, int y, char *bitmapBuf,
	short *colorTable );
void IMGbltWeakblendAreaRemap( short *imageBuf, int pitch, int x, int y, char *bitmapBuf,
	int srcX1, int srcY1, int srcX2, int srcY2, short *colorTable );
void IMGbltWeakblendAreaRemapHMirror( short *imageBuf, int pitch, int x, int y, char *bitmapBuf,
	int srcX1, int srcY1, int srcX2, int srcY2, short *colorTable );

// 16->16 blt

void IMGreadW( short *imageBuf, int pitch, int x1, int y1, int x2, int y2, short *bitmapBuf );
void IMGbltW( short *imageBuf, int pitch, int x, int y, short *bitmapBuf );
void IMGbltWArea( short *imageBuf, int pitch, int x, int y, short *bitmapBuf,
	int srcX1, int srcY1, int srcX2, int srcY2 );
void IMGscrollW( short *imageBuf, int truePitch, int srcX1, int srcY1, int srcX2, int srcY2,
	int destX1, int destY1 );	// see scroll_buffer() for complete usage

// functions with transparent color keying

void IMGbltWTrans( short *imageBuf, int pitch, int x, int y, short *bitmapBuf );
void IMGbltWAreaTrans( short *imageBuf, int pitch, int x, int y, short *bitmapBuf,
	int srcX1, int srcY1, int srcX2, int srcY2 );

// ##### begin Gilbert 20/10 #######//
// functions with drawing black mask

inline void IMGbltWTransBlacken( short *imageBuf, int pitch, int x, int y, short *bitmapWBuf ) { /* unused */ }
inline void IMGbltWAreaTransBlacken( short *imageBuf, int pitch, int x, int y, short *bitmapWBuf,
	int srcX1, int srcY1, int srcX2, int srcY2 ) { /* unused */ }
// ##### end Gilbert 20/10 #######//

// 16 bit buffer to 16 bit buffer blt

void IMGcopyW( short *imageBuf, int imgPitch, short *backBuf, int backPitch,
	int srcX1, int srcY1, int srcX2, int srcY2);
void IMGbrightBar( short *imageBuf, int pitch, int x1, int y1, int x2, int y2, int brightness);
inline void IMGcopyWbright( short *imageBuf, int imgPitch, short *backBuf, int backPitch,
	int srcX1, int srcY1, int srcX2, int srcY2, int brightness) { /* unused */ }

// join bitmap and back buffer to front buffer

void IMGjoinTransRemap( short *imageBuf, int imgPitch, short *backBuf, int backPitch,
	int x, int y, char *bitmapBuf, short *colorRemapTable );

#endif
