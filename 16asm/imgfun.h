// Filename    : IMGFUN.H
// Description : Declaration of assembly functions

#ifndef __IMGFUN_H
#define __IMGFUN_H

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
	void	INITeffect(int pixelFormatFlag);

	// 8->16 blt

	void _stdcall IMGbltRemap( short *imageBuf, int pitch, int x, int y, char *bitmapBuf,
		short *colorTable );
	void _stdcall IMGbltRemapHMirror( short *imageBuf, int pitch, int x, int y, char *bitmapBuf,
		short *colorTable );
	void _stdcall IMGbltAreaRemap( short *imageBuf, int pitch, int x, int y, char *bitmapBuf,
		int srcX1, int srcY1, int srcX2, int srcY2, short *colorTable );
	void _stdcall IMGbltAreaRemapHMirror( short *imageBuf, int pitch, int x, int y, char *bitmapBuf,
		int srcX1, int srcY1, int srcX2, int srcY2, short *colorTable );

	// functions with transparent color keying

	void _stdcall IMGbltTransRemap( short *imageBuf, int pitch, int x, int y, char *bitmapBuf,
		short *colorRemapTable );
	void _stdcall IMGblt( short *imageBuf, int pitch, int x, int y, char *bitmapBuf,
		short *colorRemapTable );
	void _stdcall IMGbltAreaTransRemap( short *imageBuf, int pitch, int x, int y, char *bitmapBuf,
		int srcX1, int srcY1, int srcX2, int srcY2, short *colorTable );
	void _stdcall IMGbltAreaTransRemapHMirror( short *imageBuf, int pitch, int x, int y, char *bitmapBuf,
		int srcX1, int srcY1, int srcX2, int srcY2, short *colorTable);

	// functions with run-length transparent key decompression

	void _stdcall IMGbltTransRemapDecompress( short *imageBuf, int pitch, int x, int y, char *bitmapBuf,
		short *colorTable);
	void _stdcall IMGbltTransRemapDecompressHMirror( short *imageBuf, int pitch, int x, int y, char *bitmapBuf,
		short *colorTable);
	void _stdcall IMGbltAreaTransRemapDecompress( short *imageBuf, int pitch, int x, int y, char *bitmapBuf,
		int srcX1, int srcY1, int srcX2, int srcY2, short *colorTable);
	void _stdcall IMGbltAreaTransRemapDecompressHMirror( short *imageBuf, int pitch, int x, int y, char *bitmapBuf,
		int srcX1, int srcY1, int srcX2, int srcY2, short *colorTable);



	// 16->16 blt

	void _stdcall IMGreadW( short *imageBuf, int pitch, int x1, int y1, int x2, int y2, short *bitmapBuf );
	void _stdcall IMGbltW( short *imageBuf, int pitch, int x, int y, short *bitmapBuf );
	void _stdcall IMGbltWArea( short *imageBuf, int pitch, int x, int y, short *bitmapBuf,
		int srcX1, int srcY1, int srcX2, int srcY2 );

	// functions with transparent color keying

	void _stdcall IMGbltWTrans( short *imageBuf, int pitch, int x, int y, short *bitmapBuf );
	void _stdcall IMGbltWAreaTrans( short *imageBuf, int pitch, int x, int y, short *bitmapBuf,
		int srcX1, int srcY1, int srcX2, int srcY2 );

	// 16 bit buffer to 16 bit buffer blt

	void _stdcall IMGcopyW( short *imageBuf, int imgPitch, short *backBuf, int backPitch,
		int srcX1, int srcY1, int srcX2, int srcY2);

	// join bitmap and back buffer to front buffer

	void _stdcall IMGjoinTransRemap( short *imageBuf, int imgPitch, short *backBuf, int backPitch,
		int x, int y, char *bitmapBuf, short *colorRemapTable );
};

#endif