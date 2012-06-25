/*
 * Seven Kingdoms 2: The Fryhtan War
 *
 * Copyright 1999 Enlight Software Ltd.
 * Copyright 2010 Jesse Allen
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

//Filename    : surface_ddraw.cpp
//Description : direct draw surface class

#include <surface_ddraw.h>
#include <all.h>
#include <ocoltbl.h>
#include <omodeid.h>
#include <ovga.h>
#include <dbglog.h>

DBGLOG_DEFAULT_CHANNEL(SurfaceDDraw);


//-------- Begin of function SurfaceDDraw::SurfaceDDraw ----------//

SurfaceDDraw::SurfaceDDraw(LPDIRECTDRAWSURFACE4 surface) :
	cur_buf_ptr(NULL), buf_locked(0), cur_pitch(0), dd_buf(surface)
{
	memset( &buf_des, 0, sizeof(buf_des) );
}
//-------- End of function SurfaceDDraw::SurfaceDDraw ----------//


//-------- Begin of function SurfaceDDraw::~SurfaceDDraw ----------//

SurfaceDDraw::~SurfaceDDraw()
{
	if( dd_buf )
	{
		if( buf_locked )
			unlock_buf();

		dd_buf->Release();
		dd_buf = NULL;
	}
}
//-------- End of function SurfaceDDraw::~SurfaceDDraw ----------//


//-------- Begin of function SurfaceDDraw::is_buf_lost ----------//
//
BOOL SurfaceDDraw::is_buf_lost()
{
	return dd_buf && dd_buf->IsLost() == DDERR_SURFACELOST;
}
//--------- End of function SurfaceDDraw::is_buf_lost ----------//


//-------- Begin of function SurfaceDDraw::restore_buf ----------//
//
// Restore buffers that have been lost.
//
BOOL SurfaceDDraw::restore_buf()
{
	if( dd_buf == NULL || dd_buf->Restore() != DD_OK )
	{
		 ERR("Error restoring direct draw buffer");
		 return FALSE;
	}

	return TRUE;
}
//--------- End of function SurfaceDDraw::restore_buf ----------//


//------------- Begin of function SurfaceDDraw::lock_buf --------------//

int SurfaceDDraw::lock_buf()
{
	err_if( buf_locked )
		err_now( "SurfaceDDraw::lock_buf() error, buffer already locked." );

	memset( &buf_des, 0, sizeof(buf_des) );

	buf_des.dwSize = sizeof(buf_des);

	int rc = dd_buf->Lock(NULL, &buf_des, DDLOCK_WAIT | DDLOCK_NOSYSLOCK, NULL);

	cur_buf_ptr = (short *) buf_des.lpSurface;
	cur_pitch = buf_des.lPitch;

	//--------------------------------------//

	if( rc==DD_OK )
		buf_locked = TRUE;

	return buf_locked;
}
//--------------- End of function SurfaceDDraw::lock_buf --------------//


//------------- Begin of function SurfaceDDraw::unlock_buf --------------//

int SurfaceDDraw::unlock_buf()
{
	// ####### begin Gilbert 16/9 #####//
	if( !dd_buf )
		return 0;
	// ####### end Gilbert 16/9 #####//
	err_when( !buf_locked );

	int rc = dd_buf->Unlock(NULL);

	if( rc==DD_OK )
		buf_locked = FALSE;

	return !buf_locked;
}
//--------------- End of function SurfaceDDraw::unlock_buf --------------//


//-------- Begin of function SurfaceDDraw::blt_buf_area --------//
//
// Blit an area of the source surface to local destination surface.
//
void SurfaceDDraw::blt_buf_area( SurfaceDDraw *srcBuf, int x1, int y1, int x2, int y2 )
{
	RECT bltRect;

	bltRect.left   = x1;
	bltRect.top    = y1;
	bltRect.right  = x2;
	bltRect.bottom = y2;

	dd_buf->BltFast( x1, y1,
			 srcBuf->dd_buf,        // src surface
			 &bltRect,              // src rect (all of it)
			 DDBLTFAST_NOCOLORKEY );
}
//--------- End of function SurfaceDDraw::blt_buf_area ---------//


//-------- Begin of function SurfaceDDraw::blt_virtual_buf --------//
//
// Blit entire source surface to local destination surface.
//
void SurfaceDDraw::blt_virtual_buf( SurfaceDDraw *srcBuf )
{
	RECT bltRect;

	bltRect.left   = 0;
	bltRect.top    = 0;
	// ##### begin Gilbert 29/1 #######//
	bltRect.right  = VGA_WIDTH;
	bltRect.bottom = VGA_HEIGHT;
	// ##### end Gilbert 29/1 #######//

	dd_buf->BltFast( 0, 0,
			 srcBuf->dd_buf,        // src surface
			 &bltRect,              // src rect (all of it)
			 DDBLTFAST_WAIT );
}
//--------- End of function SurfaceDDraw::blt_virtual_buf ---------//


//-------- Begin of function SurfaceDDraw::blt_virtual_buf_area --------//
//
// Blit an area of the source surface to local destination surface.
//
void SurfaceDDraw::blt_virtual_buf_area( SurfaceDDraw *srcBuf, int x1, int y1, int x2, int y2 )
{
	RECT bltRect;

	bltRect.left   = x1;
	bltRect.top    = y1;
	bltRect.right  = x2+1;
	bltRect.bottom = y2+1;

	dd_buf->BltFast( x1, y1,
			 srcBuf->dd_buf,        // src surface
			 &bltRect,              // src rect (all of it)
			 DDBLTFAST_WAIT );
}
//--------- End of function SurfaceDDraw::blt_virtual_buf_area ---------//


//------------ Begin of function SurfaceDDraw::write_bmp_file --------------//
//
// Load a BMP file into the current SurfaceDDraw DIB object.
//
// <char*> fileName - the name of the BMP file.
//
// return : <int> 1-succeeded, 0-failed.
//
int SurfaceDDraw::write_bmp_file(char* fileName)
{
	 File				bmpFile;
	 BITMAPINFO*	bmpInfoPtr = NULL;
	 char*			bitmapPtr = NULL;

	 int				hasPaletteFlag = 0;

	 bmpFile.file_create(fileName, 1, 0);		// 1-handle error, 0-disable variable file size

	 //------------ Write the file header ------------//

	 BITMAPFILEHEADER bmpFileHdr;

	 bmpFileHdr.bfType 		= 0x4D42;			// set the type to "BM"
	 bmpFileHdr.bfSize 		= buf_size();
	 bmpFileHdr.bfReserved1 = 0;
	 bmpFileHdr.bfReserved2 = 0;
	 bmpFileHdr.bfOffBits   = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	 if( hasPaletteFlag )
		bmpFileHdr.bfOffBits += sizeof(RGBQUAD)*256;

	 bmpFile.file_write(&bmpFileHdr, sizeof(bmpFileHdr));

	 //------------ Write in the info header -----------//

	 BITMAPINFOHEADER bmpInfoHdr;

	 bmpInfoHdr.biSize			 = sizeof(BITMAPINFOHEADER);
	 bmpInfoHdr.biWidth			 = buf_des.dwWidth;
	 bmpInfoHdr.biHeight			 = buf_des.dwHeight;
	 bmpInfoHdr.biPlanes			 = 1;
	 bmpInfoHdr.biBitCount		 = 24;
    bmpInfoHdr.biCompression	 = BI_RGB;
	 bmpInfoHdr.biSizeImage	    = bmpInfoHdr.biWidth * bmpInfoHdr.biHeight * bmpInfoHdr.biBitCount / 8;
	 bmpInfoHdr.biXPelsPerMeter = 0;
    bmpInfoHdr.biYPelsPerMeter = 0;
	 bmpInfoHdr.biClrUsed		 = 0;
    bmpInfoHdr.biClrImportant  = 0;

	 bmpFile.file_write(&bmpInfoHdr, sizeof(bmpInfoHdr));

	 //------------ write the color table -----------//

	 if( hasPaletteFlag )
	 {
		 LPDIRECTDRAWPALETTE ddPalettePtr;				// get the direct draw surface's palette
		 dd_buf->GetPalette(&ddPalettePtr);

		 PALETTEENTRY *palEntries = (PALETTEENTRY*) mem_add( sizeof(PALETTEENTRY)*256 );
		 ddPalettePtr->GetEntries(0, 0, 256, palEntries);

		 RGBQUAD *colorTable = (RGBQUAD*) mem_add( sizeof(RGBQUAD)*256 );		// allocate a color table with 256 entries

		 for( int i=0 ; i<256 ; i++ )
		 {
			 colorTable[i].rgbBlue  = palEntries[i].peBlue;
			 colorTable[i].rgbGreen = palEntries[i].peGreen;
			 colorTable[i].rgbRed   = palEntries[i].peRed;
			 colorTable[i].rgbReserved = 0;
		 }

		 bmpFile.file_write(colorTable, sizeof(RGBQUAD)*256);

		 mem_del(palEntries);
		 mem_del(colorTable);
	 }

	 //----------- write the bitmap ----------//

	 if( bmpInfoHdr.biBitCount == 8 )
	 {
		 for( int y=buf_height()-1 ; y>=0 ; y-- )					// write in reversed order as DIB's vertical order is reversed
			 bmpFile.file_write(buf_ptr(0,y), buf_width());
	 }
	 else if( bmpInfoHdr.biBitCount == 24 )
	 {
		 int lineBufferSize = sizeof(RGBColor) * bmpInfoHdr.biWidth;
		 RGBColor *lineBuffer = (RGBColor *)mem_add( lineBufferSize );
		 for( int y = buf_height()-1; y>=0 ; --y )
		 {
			 register short *pixelPtr = buf_ptr( 0, y );
			 register RGBColor *lineBufPtr = lineBuffer;
			 for( int x = buf_width()-1; x >= 0; --x, ++pixelPtr, ++lineBufPtr)
			 {
				 vga.decode_pixel( *pixelPtr, lineBufPtr );
				 // exchange Red and blue
				 BYTE r = lineBufPtr->red;
				 lineBufPtr->red = lineBufPtr->blue;
				 lineBufPtr->blue = r;
			 }
			 bmpFile.file_write(lineBuffer, lineBufferSize );
		 }
		 mem_del(lineBuffer);
	 }
	 else
	 {
		 err_here();
	 }

	 //------------ close the file -----------//

	 bmpFile.file_close();

	 return 1;
}
//------------ End of function SurfaceDDraw::write_bmp_file --------------//
