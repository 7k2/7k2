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

//Filename    : VGAFUN.H
//Description : Header file for image manipulation functions

#ifndef __VGAFUN_H
#define __VGAFUN_H

//------- Declare external functions ---------//

extern "C"
{
	// not used : void _stdcall IMGinit(int,int);
	void _stdcall IMGbar(char*,int pitch,int,int,int,int,int);
	void _stdcall IMGread(char*,int pitch,int,int,int,int,char*);
	void _stdcall IMGblack32x32(char*,int pitch,int,int);

	void _stdcall IMGblt(char*,int pitch,int,int,char*);
	void _stdcall IMGblt2(char*,int pitch,int,int,int,int,char*);
	void _stdcall IMGblt32x32(char*,int pitch,int,int,char*);
	void _stdcall IMGbltDW(char*,int pitch,int,int,char*);
	void _stdcall IMGbltTrans(char*,int pitch,int,int,char*);
	void _stdcall IMGbltTransHMirror(char*,int pitch,int,int,char*);
	void _stdcall IMGbltTransVMirror(char*,int pitch,int,int,char*);
	void _stdcall IMGbltTransHVMirror(char*,int pitch,int,int,char*);
	void _stdcall IMGbltTransRemap(char*,int pitch,int,int,char*,char*);
	void _stdcall IMGbltArea(char* imageBuf,int pitch, int desX, int desY, char* bitmapBuf, int srcX1, int srcY1, int srcX2, int srcY2);
	void _stdcall IMGbltAreaTrans(char* imageBuf, int pitch, int desX, int desY, char* bitmapBuf, int srcX1, int srcY1, int srcX2, int srcY2);
	void _stdcall IMGbltAreaTransHMirror(char* imageBuf, int pitch, int desX, int desY, char* bitmapBuf, int srcX1, int srcY1, int srcX2, int srcY2);

	void _stdcall IMGbltTransDecompressHMirror(char* imageBuf, int pitch, int desX, int desY, char* bitmapBuf);
	void _stdcall IMGbltAreaTransDecompressHMirror(char* imageBuf, int pitch, int desX, int desY, char* bitmapBuf, int srcX1, int srcY1, int srcX2, int srcY2);

	void _stdcall IMGremapDecompress(char* desPtr, char* srcPtr, char* colorTable);
	void _stdcall IMGbltTransDecompress(char* imageBuf, int pitch, int desX, int desY, char* bitmapBuf);
	void _stdcall IMGbltTransRemapDecompress(char* imageBuf, int pitch, int desX, int desY, char* bitmapBuf, char* colorTable);
	void _stdcall IMGbltTransRemapDecompressHMirror(char* imageBuf, int pitch, int desX, int desY, char* bitmapBuf, char* colorTable);
	void _stdcall IMGbltAreaTransDecompress(char* imageBuf, int pitch, int desX, int desY, char* bitmapBuf, int srcX1, int srcY1, int srcX2, int srcY2);
	void _stdcall IMGbltAreaTransRemapDecompress(char* imageBuf, int pitch, int desX, int desY, char* bitmapBuf, int srcX1, int srcY1, int srcX2, int srcY2, char* colorTable);
	void _stdcall IMGbltAreaTransRemapDecompressHMirror(char* imageBuf, int pitch, int desX, int desY, char* bitmapBuf, int srcX1, int srcY1, int srcX2, int srcY2, char* colorTable);

	void _stdcall IMGjoinTrans(char* imageBuf, int imgPitch, char* backBuf, int backPitch, int x, int y, char* bitmapPtr);
	void _stdcall IMGcopy(char* imageBuf, int imgPitch, char* backBuf, int backPitch, int x1, int y1, int x2, int y2);
	void _stdcall IMGcopyRemap(char*, int imgPitch,char*, int backPitch,int,int,int,int,unsigned char*);

	// used in wall
	void _stdcall IMGbltRemap(char* imageBuf, int pitch, int desX, int desY, char* bitmapBuf, char* colorTable);
	void _stdcall IMGbltAreaTransRemap(char* imageBuf, int pitch, int desX, int desY, char* bitmapBuf, int srcX1, int srcY1, int srcX2, int srcY2, char* colorTable);
	void _stdcall IMGbltAreaRemap(char* imageBuf, int pitch, int desX, int desY, char* bitmapBuf, int srcX1, int srcY1, int srcX2, int srcY2, char* colorTable);

	void _stdcall IMGputBitFont(char*,int pitch,int,int,int,int,int,int,char*,int,int);
	void _stdcall IMGline(char*,int pitch,int w, int h, int,int,int,int,int);
	void _stdcall IMGxor(char*,int pitch,int,int,int,int);

	void _stdcall IMGdarken(char*,int pitch,int,int,int,int,int);
	void _stdcall IMGtile(char*,int pitch,int,int,int,int,char*);
	void _stdcall IMGpixel32x32(char*,int pitch,int,int,int);

	void _stdcall IMGsnow32x32(char*,int pitch,int,int,int,int);
	void _stdcall IMGexploreMask32x32( char *,int pitch, int, int, char *, int, int, int);
	void _stdcall IMGexploreRemap32x32( char *,int pitch, int, int, char *, char **,int, int, int);
	void _stdcall IMGfogRemap32x32( char *,int pitch, int, int, char**, unsigned char*, unsigned char*, unsigned char*);

	// ----- colour remapping functions ------//
	void _stdcall IMGremapBar(char*,int pitch,int,int,int,int,unsigned char*);
	void _stdcall IMGremap(char*,int pitch,int,int,char*,unsigned char**);
	void _stdcall IMGremapHMirror(char*,int pitch,int,int,char*,unsigned char**);
	void _stdcall IMGremapArea(char*,int pitch,int,int,char*,unsigned char**,int,int,int,int);
	void _stdcall IMGremapAreaHMirror(char*,int pitch,int,int,char*,unsigned char**,int,int,int,int);
};

//-------------------------------------------//

#endif

