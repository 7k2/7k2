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

//Filename    : OVGABUF.CPP
//Description : OVGABUF direct draw surface class

#include <ddraw.h>
#include <ovgabuf.h>
#include <ovga.h>
#include <omodeid.h>
#include <all.h>
#include <omouse.h>
#include <imgfun.h>
#include <osys.h>
#include <oworld.h>
#include <obitmap.h>
#include <obitmapw.h>
#include <ocoltbl.h>
#include <stdio.h>

short *VgaBuf::default_remap_table;
short *VgaBuf::default_blend_table;

const char *dd_err_str( const char *str, HRESULT rc);



// ------- pass inline function ------//

int VgaBuf::buf_size()					{ return buf_des.dwWidth * buf_des.dwHeight * sizeof(short); }
int VgaBuf::buf_width()					{ return buf_des.dwWidth; }
int VgaBuf::buf_height()            { return buf_des.dwHeight; }
void VgaBuf::set_default_buf_ptr()	{ cur_buf_ptr = (short*)buf_des.lpSurface; cur_pitch = buf_des.lPitch; }


//-------- Begin of function VgaBuf::VgaBuf ----------//

VgaBuf::VgaBuf()
{
	memset( this, 0, sizeof(VgaBuf) );
}
//-------- End of function VgaBuf::VgaBuf ----------//


//-------- Begin of function VgaBuf::~VgaBuf ----------//

VgaBuf::~VgaBuf()
{
	deinit();
}
//-------- End of function VgaBuf::~VgaBuf ----------//


//-------- Begin of function VgaBuf::init_front ----------//
//
// Create a direct draw front buffer.
//
// <LPVOID> vdd4Ptr     LPDIRECTDRAW4 of vga
//
void VgaBuf::init_front(LPVOID vdd4Ptr)
{
	LPDIRECTDRAW4 ddPtr = (LPDIRECTDRAW4) vdd4Ptr;

	DDSURFACEDESC2       ddsd;
	HRESULT             rc;

	// check size of union structure
	err_when( sizeof(dd_buf) > sizeof(vptr_dd_buf) );
	err_when( sizeof(buf_des) > sizeof(c_buf_des) );

	// ##### begin Gilbert 4/11 ######//
	//------ Get Direct Draw capacity info --------//
//	DDCAPS              ddcaps;
//	ZeroMemory( &ddcaps, 0, sizeof(ddcaps) );
//	ddcaps.dwSize = sizeof( ddcaps );
//	if( ddPtr->GetCaps( &ddcaps, NULL ) != DD_OK )
//		err.run( "Error creating Direct Draw front surface!" );
	// ##### end Gilbert 4/11 ######//

	//---------------------------------------------//
	// Create the Front Buffer
	//---------------------------------------------//

	ZeroMemory( &ddsd, sizeof(ddsd) );
	ddsd.dwSize = sizeof( ddsd );

	ddsd.dwFlags = DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
//	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_VIDEOMEMORY;

//	LPDIRECTDRAWSURFACE dd1Buf;
//	rc = ddPtr->CreateSurface( &ddsd, &dd1Buf, NULL );
	// no createSurface can direct create direct draw surface 4 interface
	rc = ddPtr->CreateSurface( &ddsd, &dd_buf, NULL );
	if( rc != DD_OK )
	{
		err.run( dd_err_str("Error creating Direct Draw front surface!" ,rc) );
	}

//	rc = dd1Buf->QueryInterface(IID_IDirectDrawSurface2, (void **)&dd_buf);
//	if( rc != DD_OK )
//	{
//		dd1Buf->Release();
//		err.run ( dd_err_str("Error creating Direct Draw front surface!!", rc) );
//	}
//	dd1Buf->Release();

	lock_bit_stack = 0;
	lock_stack_count = 0;

	default_remap_table = vga.default_remap_table;	// new for 16-bit
	default_blend_table = vga.default_blend_table;	// new for 16-bit

	is_front = 1;
}
//-------- End of function VgaBuf::init_front ----------//


//-------- Begin of function VgaBuf::init_back ----------//
//
// Create a direct draw back buffer.
//
// <LPVOID> vdd4Ptr     LPDIRECTDRAW4 of vga
// [DWORD] w      : width of the surface [default 0 : VGA_WIDTH]
// [DWORD] h      : height of the surface [default 0 : VGA_HEIGHT]
// [int] videoMemoryFlag : 1 for create surface in video memory [default 0 : in system memory]
//
void VgaBuf::init_back( LPVOID vdd4Ptr, DWORD w, DWORD h, int videoMemoryFlag )
{
	LPDIRECTDRAW4 ddPtr = (LPDIRECTDRAW4) vdd4Ptr;

	DDSURFACEDESC2       ddsd;
	HRESULT             rc;

	// check size of union structure
	err_when( sizeof(dd_buf) > sizeof(vptr_dd_buf) );
	err_when( sizeof(buf_des) > sizeof(c_buf_des) );

	//--------- fill in surface desc -----------//

	memset( &ddsd, 0, sizeof( ddsd ) );
	ddsd.dwSize = sizeof( ddsd );
	ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT |DDSD_WIDTH;

	// create back buffer
	if( videoMemoryFlag & 1)
		ddsd.ddsCaps.dwCaps = 0; 
	//	ddsd.ddsCaps.dwCaps = DDSCAPS_VIDEOMEMORY;
	else
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;

	ddsd.dwWidth  = w ? w : VGA_WIDTH;
	ddsd.dwHeight = h ? h : VGA_HEIGHT;

//	LPDIRECTDRAWSURFACE dd1Buf;
//	rc = ddPtr->CreateSurface( &ddsd, &dd1Buf, NULL );
	// CreateSurface can now create DIRECTDRAWSURFACE4
	rc = ddPtr->CreateSurface( &ddsd, &dd_buf, NULL );
	if( rc != DD_OK )
		err.run( dd_err_str("Error creating direct draw back surface!", rc) );

//	rc = dd1Buf->QueryInterface(IID_IDirectDrawSurface2, (void **)&dd_buf);
//	if( rc != DD_OK )
//	{
//		dd1Buf->Release();
//		err.run( dd_err_str("Error creating direct draw back surface!!", rc) );
//	}
//	dd1Buf->Release();

	lock_bit_stack = 0;
	lock_stack_count = 0;

	default_remap_table = vga.default_remap_table;	// new for 16-bit
}
//-------- End of function VgaBuf::init_back ----------//


//------ Begin of function VgaBuf::attach_surface --------//
void VgaBuf::attach_surface(VgaBuf *backBuf)
{
	HRESULT rc;
	if( dd_buf && backBuf->dd_buf )
	{
		rc = dd_buf->AddAttachedSurface(backBuf->dd_buf);
		// ##### begin Gilbert 4/11 #####//
		// err_when( rc != DD_OK );
		if( rc != DD_OK )
			err.run( dd_err_str("Cannot attach flipping surface", rc) );
		// ##### end Gilbert 4/11 #####//
	}
}
//------ End of function VgaBuf::attach_surface --------//


//------ Begin of function VgaBuf::detach_surface --------//
void VgaBuf::detach_surface(VgaBuf *backBuf)
{
	HRESULT rc;
	if( dd_buf && backBuf->dd_buf )
	{
		rc = dd_buf->DeleteAttachedSurface(0, backBuf->dd_buf);
		err_when( rc != DD_OK );
	}
}
//------ End of function VgaBuf::detach_surface --------//


//------ Begin of function VgaBuf::deinit --------//

void VgaBuf::deinit()
{
	if( dd_buf )
	{
		dd_buf->Release();
		dd_buf = NULL;
	}
}
//-------- End of function VgaBuf::deinit ----------//


//-------- Begin of function VgaBuf::activate_pal ----------//
//
// Activate a palette to the current direct draw surface buffer.
//
void VgaBuf::activate_pal(LPVOID vddPalPtr)
{
	LPDIRECTDRAWPALETTE ddPalPtr = (LPDIRECTDRAWPALETTE) vddPalPtr;

	return;			// no need for 16-bit

	err_when(!ddPalPtr || !dd_buf);

	HRESULT rc = dd_buf->SetPalette(ddPalPtr);

	if( rc == DDERR_SURFACELOST )
	{
		dd_buf->Restore();
		rc = dd_buf->SetPalette(ddPalPtr);
	}

#ifdef DEBUG
	if( rc != DD_OK )
		debug_msg( "VgaBuf::activate_pal(), failed activating the palette" );
#endif
}
//--------- End of function VgaBuf::activate_pal ----------//


//-------- Begin of function VgaBuf::is_buf_lost ----------//
//
BOOL VgaBuf::is_buf_lost()
{
	return dd_buf && dd_buf->IsLost() == DDERR_SURFACELOST;
}
//--------- End of function VgaBuf::is_buf_lost ----------//


//-------- Begin of function VgaBuf::restore_buf ----------//
//
// Restore buffers that have been lost.
//
BOOL VgaBuf::restore_buf()
{
	if( dd_buf == NULL || dd_buf->Restore() != DD_OK )
	{
#ifdef DEBUG
		 debug_msg("Error restoring direct draw buffer");
#endif
		 return FALSE;
	}

	return TRUE;
}
//--------- End of function VgaBuf::restore_buf ----------//


//------------- Begin of function VgaBuf::lock_buf --------------//

void VgaBuf::lock_buf()
{
	err_if( buf_locked )
		err_now( "VgaBuf::lock_buf() error, buffer already locked." );

	memset( &buf_des, 0, sizeof(buf_des) );

	buf_des.dwSize = sizeof(buf_des);

	int rc = dd_buf->Lock(NULL, &buf_des, DDLOCK_WAIT | DDLOCK_NOSYSLOCK, NULL);

	cur_buf_ptr = (short *) buf_des.lpSurface;
	cur_pitch = buf_des.lPitch;

	//--------------------------------------//

	if( rc==DD_OK )
		buf_locked = TRUE;
	else
	{
		if( is_front )
			err_now( dd_err_str("VgaBuf::lock_buf() locking front buffer failed.", rc) );
		else
			err_now( dd_err_str("VgaBuf::lock_buf() locking back buffer failed.", rc) );

#ifdef DEBUG
		debug_msg( "Failed to lock the buffer." );
#endif
	}
}
//--------------- End of function VgaBuf::lock_buf --------------//


//------------- Begin of function VgaBuf::unlock_buf --------------//

void VgaBuf::unlock_buf()
{
	// ####### begin Gilbert 16/9 #####//
	if( !dd_buf )
		return;
	// ####### end Gilbert 16/9 #####//
	err_when( !buf_locked );

	int rc = dd_buf->Unlock(NULL);

	if( rc==DD_OK )
		buf_locked = FALSE;
	else
	{
		if( is_front )
			err_now( dd_err_str("VgaBuf::unlock_buf() unlocking front buffer failed.", rc) );
		else
			err_now( dd_err_str("VgaBuf::unlock_buf() unlocking back buffer failed.", rc) );

#ifdef DEBUG
		debug_msg( "Failed to unlock the buffer." );
#endif
	}
}
//--------------- End of function VgaBuf::unlock_buf --------------//


//------------- Begin of function VgaBuf::temp_unlock --------------//
//
// Unlock the Vga buffer temporarily.
//
void VgaBuf::temp_unlock()
{
	// push buf_locked
	err_when(lock_stack_count >= sizeof(lock_bit_stack)*8);
	err_when(buf_locked != 0 && buf_locked != 1);	// 0 or 1
	lock_bit_stack = (lock_bit_stack << 1) | buf_locked;
	++lock_stack_count;

	if( buf_locked )
		unlock_buf();

	err_when( buf_locked );
}
//--------------- End of function VgaBuf::temp_unlock --------------//


//------------- Begin of function VgaBuf::temp_restore_lock --------------//
//
// Restore the previous lock stage.
// paired with temp_unlock()
//
void VgaBuf::temp_restore_lock()
{
	// pop buf_locked
	err_when(lock_stack_count==0);
	DWORD newBufLocked = lock_bit_stack & 1;
	lock_bit_stack >>= 1;
	lock_stack_count--;
	
	if( newBufLocked )
		lock_buf();
}
//--------------- End of function VgaBuf::temp_restore_lock --------------//



//------------- Begin of function VgaBuf::temp_lock --------------//
//
// Lock the Vga buffer temporarily.
//
void VgaBuf::temp_lock()
{
	// push buf_locked
	err_when(lock_stack_count >= sizeof(lock_bit_stack)*8);
	err_when(buf_locked != 0 && buf_locked != 1);	// 0 or 1
	lock_bit_stack = (lock_bit_stack << 1) | buf_locked;
	++lock_stack_count;

	if( !buf_locked )
		lock_buf();
	err_when( !buf_locked );
}
//------------- End of function VgaBuf::temp_lock --------------//


//------------- Begin of function VgaBuf::temp_restore_unlock --------------//
//
// Restore the previous unlock stage.
// paired with temp_lock()
//
void VgaBuf::temp_restore_unlock()
{
	// pop buf_locked
	err_when(lock_stack_count==0);
	DWORD newBufLocked = lock_bit_stack & 1;
	lock_bit_stack >>= 1;
	lock_stack_count--;
	
	if( !newBufLocked )
		unlock_buf();
}
//------------- End of function VgaBuf::temp_restore_unlock --------------//



//------------- Begin of function VgaBuf::put_bitmap --------------//
//
// Put a bitmap on the surface buffer
//
void VgaBuf::put_bitmap(int x,int y,char* bitmapPtr)
{ 
	err_when( !buf_locked );

	if( is_front )
		mouse.hide_area( x, y, x+*((short*)bitmapPtr)-1, y+*(((short*)bitmapPtr)+1)-1 );

	IMGbltRemap(cur_buf_ptr, cur_pitch, x, y, bitmapPtr, default_remap_table);

	if( is_front )
		mouse.show_area();
}
//--------------- End of function VgaBuf::put_bitmap --------------//


//------- Begin of function VgaBuf::put_bitmap_trans --------//
//
// Put a bitmap on the surface buffer and hide the mouse cursor while displaying
//
void VgaBuf::put_bitmap_trans(int x,int y,char* bitmapPtr)
{
	err_when( !buf_locked );

	if( is_front )
		mouse.hide_area( x, y, x+*((short*)bitmapPtr)-1, y+*(((short*)bitmapPtr)+1)-1 );

	IMGbltTransRemap(cur_buf_ptr, cur_pitch, x, y, bitmapPtr, default_remap_table);

	if( is_front )
		mouse.show_area();
}
//--------- End of function VgaBuf::put_bitmap_trans --------//


//------- Begin of function VgaBuf::put_bitmap_remap --------//
//
// Put a bitmap on the surface buffer and hide the mouse cursor while displaying
//
void VgaBuf::put_bitmap_remap(int x,int y,char* bitmapPtr,short *colorTable)
{
	err_when( !buf_locked );

	if( is_front )
		mouse.hide_area( x, y, x+((Bitmap *)bitmapPtr)->get_width()-1, y+((Bitmap*)bitmapPtr)->get_height()-1 );

	IMGbltRemap(cur_buf_ptr, cur_pitch, x, y, bitmapPtr, colorTable);

	if( is_front )
		mouse.show_area();
}
//--------- End of function VgaBuf::put_bitmap_remap --------//


//------- Begin of function VgaBuf::put_bitmapW --------//
//
void VgaBuf::put_bitmapW(int x, int y, short *bitmapWBuf )
{
	err_when( !buf_locked );

	if( is_front )
		mouse.hide_area( x, y, x+((BitmapW *)bitmapWBuf)->get_width()-1, y+((BitmapW *)bitmapWBuf)->get_height()-1 );

	IMGbltW( cur_buf_ptr, cur_pitch, x, y, bitmapWBuf );

	if( is_front )
		mouse.show_area();
}
//------- End of function VgaBuf::put_bitmapW --------//


//---------- Begin of function VgaBuf::save_area_common_buf ----------//
//
// Save screen area to sys.common_data_buf.
//
void VgaBuf::save_area_common_buf(int x1, int y1, int x2, int y2)
{
	err_when( x1>x2 || y1>y2 || x1<0 || y1<0 || x2>=VGA_WIDTH || y2>=VGA_HEIGHT );

	long saveSize = sizeof(short)*4 + BitmapW::size(x2-x1+1, y2-y1+1);

	err_if( saveSize > COMMON_DATA_BUF_SIZE )
		err_now( "VgaBuf::save_area_common_buf()" );

	short* shortPtr = (short*) sys.common_data_buf;

	*shortPtr++ = x1;
	*shortPtr++ = y1;
	*shortPtr++ = x2;
	*shortPtr++ = y2;

	//-------- read screen ---------//

	if( is_front )
		mouse.hide_area( x1,y1,x2,y2 );  // if the mouse cursor is in that area, hide it

	read_bitmapW( x1,y1,x2,y2, shortPtr );

	if( is_front )
		mouse.show_area();
}
//------------ End of function VgaBuf::save_area_common_buf ----------//


//---------- Begin of function VgaBuf::rest_area_common_buf ----------//
//
// Restore screen area to the screen from Vga image buffer.
// This screen should be previously saved by save_area()
//
void VgaBuf::rest_area_common_buf()
{
	short* shortPtr = (short*) sys.common_data_buf;

	int x1 = *shortPtr++;
	int y1 = *shortPtr++;
	int x2 = *shortPtr++;
	int y2 = *shortPtr++;

	// ##### begin Gilbert 30/10 ########//
	put_bitmapW( x1, y1, shortPtr );
	// ##### end Gilbert 30/10 ########//
}
//------------ End of function VgaBuf::rest_area_common_buf ----------//


//---------- Begin of function VgaBuf::save_area ---------//
//
// save_area() differs from save_area() as :
//
// save_area() save the screen to a user-defined buffer.
// save_area()  save the screen to the global screen saving buffer in Vga object
//
// <int>   x1,y1,x2,y2 = the area of the screen
// [char*] saveScr     = the pointer to the previous saved buffer
//                       (default : initialize a new buffer)
//
short* VgaBuf::save_area(int x1, int y1, int x2, int y2, short* saveScr )
{
	err_when( x1>x2 || y1>y2 || x1<0 || y1<0 || x2>=VGA_WIDTH || y2>=VGA_HEIGHT );

	long newSize = sizeof(short)*4 + BitmapW::size((x2-x1+1),(y2-y1+1));

	saveScr = (short *)mem_resize( saveScr, newSize );

	short* shortPtr = (short*) saveScr;

	*shortPtr++ = x1;
	*shortPtr++ = y1;
	*shortPtr++ = x2;
	*shortPtr++ = y2;

	if( is_front )
		mouse.hide_area( x1,y1,x2,y2 );  // if the mouse cursor is in that area, hide it

	read_bitmapW( x1,y1,x2,y2, shortPtr );

	if( is_front )
		mouse.show_area();

   return saveScr;
}
//------------ End of function VgaBuf::save_area ---------//


//----------- Begin of function VgaBuf::rest_area --------//
//
// Restore previously saved screen
//
// char* saveScr     = the previously saved screen
// [int] releaseFlag = whether release the buffer of the saved screen or not
//                     (default : 1)
//
void VgaBuf::rest_area(short* saveScr, int releaseFlag)
{
   int  x1,y1,x2,y2;

   if( saveScr == NULL )
      return;

   short* shortPtr = (short*) saveScr;

	x1 = *shortPtr++;
   y1 = *shortPtr++;
   x2 = *shortPtr++;
   y2 = *shortPtr++;

   err_when( x1>x2 || y1>y2 || x1<0 || y1<0 || x2>=VGA_WIDTH || y2>=VGA_HEIGHT );

	// ##### begin Gilbert 30/10 ########//
	put_bitmapW( x1, y1, shortPtr );
	// ##### end Gilbert 30/10 ########//

	if( releaseFlag )
		mem_del( saveScr );
}
//------------ End of function VgaBuf::rest_area ----------//


//------------ Begin of function VgaBuf::write_bmp_file --------------//
//
// Load a BMP file into the current VgaBuf DIB object.
//
// <char*> fileName - the name of the BMP file.
//
// return : <int> 1-succeeded, 0-failed.
//
int VgaBuf::write_bmp_file(char* fileName)
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
//------------ End of function VgaBuf::write_bmp_file --------------//


//---------- Begin of function VgaBuf::put_large_bitmap ---------//
//
// Put a picture on the screen, when a picture's size is > 64K
// Pass a file pointer to put_large_bitmap() for continously reading the file
//
// Syntax
//
// <int>   x, y    = the location of the picture on the screen
// <FILE*> filePtr = pointer to the
//
//--------- Format of Picture buffer -------//
//
// <int> = picture width
// <int> = picture height
//
// char[...]    = non-compressed flat picture bitmap
//
//---------------------------------------------//

void VgaBuf::put_large_bitmap(int x1, int y1, File* filePtr, short *colorRemapTable)
{
	if( filePtr == NULL )
		return;

	if( !colorRemapTable )
		colorRemapTable = default_remap_table;

	int pictWidth = filePtr->file_get_short();
	int hasPalette=0;

	//------ read in bitmap and display it --------//

	int pictHeight = filePtr->file_get_short();
	int x2 = x1 + pictWidth  - 1;
	int y2 = y1 + pictHeight - 1;

	long pictSize = (long) pictWidth * pictHeight;

	err_when( x1>x2 || y1>y2 || x1<0 || y1<0 || x2>=VGA_WIDTH || y2>=VGA_HEIGHT );

	//---- if pict size less than 64K, read in the picture in one step ----//

	if( Bitmap::size(pictWidth, pictHeight) <= COMMON_DATA_BUF_SIZE )
	{
		((Bitmap *)sys.common_data_buf)->init(pictWidth, pictHeight);

		filePtr->file_read( ((Bitmap *)sys.common_data_buf)->bitmap, pictSize );

		if( is_front )
			mouse.hide_area( x1,y1,x2,y2 );  // if the mouse cursor is in that area, hide it

		put_bitmap_remap_fast( x1, y1, (char *)sys.common_data_buf, colorRemapTable );

		if( is_front )
			mouse.show_area();
	}
	else //----- if the picture size > 64K, read in line by line -----//
	{
		int bufferLine = (COMMON_DATA_BUF_SIZE - 2*sizeof(short) )/ pictWidth;   // max. no. of lines can be in the buffer
		int ty=y1+bufferLine-1;

		if( ty> y2 )
			ty=y2;

		while( y1<=y2 )
		{
			((Bitmap *)sys.common_data_buf)->init( pictWidth, (ty-y1+1) );
			filePtr->file_read( ((Bitmap *)sys.common_data_buf)->bitmap, (unsigned)pictWidth * (ty-y1+1) );

			if( is_front )
				mouse.hide_area( x1,y1,x2,ty );  // if the mouse cursor is in that area, hide it

			put_bitmap_remap_fast( x1, y1, sys.common_data_buf, colorRemapTable );

			if( is_front )
				mouse.show_area();

			y1 += bufferLine;

			if( (ty+=bufferLine) > y2 )
				ty=y2;
		}
	}
}
//----------- End of function VgaBuf::put_large_bitmap --------//


//---------- Begin of function VgaBuf::put_large_bitmap_trans ---------//
//
// Put a picture on the screen, when a picture's size is > 64K
// Pass a file pointer to put_large_bitmap_trans() for continously reading the file
//
// Syntax
//
// <int>   x, y    = the location of the picture on the screen
// <FILE*> filePtr = pointer to the
//
//--------- Format of Picture buffer -------//
//
// <int> = picture width
// <int> = picture height
//
// char[...]    = non-compressed flat picture bitmap
//
//---------------------------------------------//

void VgaBuf::put_large_bitmap_trans(int x1, int y1, File* filePtr, short *colorRemapTable)
{
	if( filePtr == NULL )
		return;

	if( !colorRemapTable )
		colorRemapTable = default_remap_table;

	int pictWidth = filePtr->file_get_short();
	int hasPalette=0;

	//------ read in bitmap and display it --------//

	int pictHeight = filePtr->file_get_short();
	int x2 = x1 + pictWidth  - 1;
	int y2 = y1 + pictHeight - 1;

	long pictSize = (long) pictWidth * pictHeight;

	err_when( x1>x2 || y1>y2 || x1<0 || y1<0 || x2>=VGA_WIDTH || y2>=VGA_HEIGHT );

	//---- if pict size less than 64K, read in the picture in one step ----//

	if( Bitmap::size(pictWidth, pictHeight) <= COMMON_DATA_BUF_SIZE )
	{
		((Bitmap *)sys.common_data_buf)->init(pictWidth, pictHeight);

		filePtr->file_read( ((Bitmap *)sys.common_data_buf)->bitmap, pictSize );

		if( is_front )
			mouse.hide_area( x1,y1,x2,y2 );  // if the mouse cursor is in that area, hide it

		put_bitmap_trans_remap( x1, y1, (char *)sys.common_data_buf, colorRemapTable );

		if( is_front )
			mouse.show_area();
	}
	else //----- if the picture size > 64K, read in line by line -----//
	{
		int bufferLine = (COMMON_DATA_BUF_SIZE - 2*sizeof(short) )/ pictWidth;   // max. no. of lines can be in the buffer
		int ty=y1+bufferLine-1;

		if( ty> y2 )
			ty=y2;

		while( y1<=y2 )
		{
			((Bitmap *)sys.common_data_buf)->init( pictWidth, (ty-y1+1) );
			filePtr->file_read( ((Bitmap *)sys.common_data_buf)->bitmap, (unsigned)pictWidth * (ty-y1+1) );

			if( is_front )
				mouse.hide_area( x1,y1,x2,ty );  // if the mouse cursor is in that area, hide it

			put_bitmap_trans_remap( x1, y1, sys.common_data_buf, colorRemapTable );

			if( is_front )
				mouse.show_area();

			y1 += bufferLine;

			if( (ty+=bufferLine) > y2 )
				ty=y2;
		}
	}
}
//----------- End of function VgaBuf::put_large_bitmap_trans --------//


//---------- Begin of function VgaBuf::put_large_bitmapW ---------//
//
// Put a picture on the screen, when a picture's size is > 64K
// Pass a file pointer to put_large_bitmap() for continously reading the file
//
// Syntax
//
// <int>   x, y    = the location of the picture on the screen
// <FILE*> filePtr = pointer to the
//
//--------- Format of Picture buffer -------//
//
// <int> = picture width
// <int> = picture height
//
// short[...]    = non-compressed flat picture bitmap
//
//---------------------------------------------//

void VgaBuf::put_large_bitmapW(int x1, int y1, File* filePtr)
{
	if( filePtr == NULL )
		return;

	int pictWidth = filePtr->file_get_short();

	//------ read in bitmap and display it --------//

	int pictHeight = filePtr->file_get_short();
	int x2 = x1 + pictWidth  - 1;
	int y2 = y1 + pictHeight - 1;

	long pictSize = (long) pictWidth * pictHeight * sizeof(short);

	err_when( x1>x2 || y1>y2 || x1<0 || y1<0 || x2>=VGA_WIDTH || y2>=VGA_HEIGHT );

	//---- if pict size less than 64K, read in the picture in one step ----//

	if( BitmapW::size(pictWidth, pictHeight) <= COMMON_DATA_BUF_SIZE )
	{
		((BitmapW *)sys.common_data_buf)->init(pictWidth, pictHeight);

		filePtr->file_read( ((BitmapW *)sys.common_data_buf)->bitmap, pictSize );

		// ##### begin Gilbert 30/10 ######//
		put_bitmapW( x1, y1, (short *)sys.common_data_buf );
		// ##### end Gilbert 30/10 ######//
	}
	else //----- if the picture size > 64K, read in line by line -----//
	{
		int bufferLine = (COMMON_DATA_BUF_SIZE - 2*sizeof(short) )/ pictWidth / sizeof(short);   // max. no. of lines can be in the buffer
		int ty=y1+bufferLine-1;

		if( ty> y2 )
			ty=y2;

		while( y1<=y2 )
		{
			((BitmapW *)sys.common_data_buf)->init( pictWidth, (ty-y1+1) );
			filePtr->file_read( ((BitmapW *)sys.common_data_buf)->bitmap, (unsigned)pictWidth * (ty-y1+1) * sizeof(short) );

			// ##### begin Gilbert 30/10 ######//
			put_bitmapW( x1, y1, (short *)sys.common_data_buf);
			// ##### end Gilbert 30/10 ######//

			y1 += bufferLine;

			if( (ty+=bufferLine) > y2 )
				ty=y2;
		}
	}
}
//----------- End of function VgaBuf::put_large_bitmapW --------//


//----------- Begin of function VgaBuf::convert_gray ----------//
//
// Convert an specified area of the bitmap from color to gray-scale.
//
void VgaBuf::convert_gray(int x1, int y1, int x2, int y2)
{
	// remap_bar(x1, y1, x2, y2, vga.gray_remap_table);
}
//--------- End of function VgaBuf::convert_gray -----------//



const char *dd_err_str( const char *s, HRESULT rc)
{
	static char retStr[200];
	const char *errStr = NULL;

	switch(rc)
	{
	case DDERR_ALREADYINITIALIZED:
		errStr = "DDERR_ALREADYINITIALIZED";
		break;
	case DDERR_CANNOTATTACHSURFACE:
		errStr = "DDERR_CANNOTATTACHSURFACE";
		break;
	case DDERR_CANNOTDETACHSURFACE:
		errStr = "DDERR_CANNOTDETACHSURFACE";
		break;
	case DDERR_CURRENTLYNOTAVAIL:
		errStr = "DDERR_CURRENTLYNOTAVAIL";
		break;
	case DDERR_EXCEPTION:
		errStr = "DDERR_EXCEPTION";
		break;
	case DDERR_GENERIC:
		errStr = "DDERR_GENERIC";
		break;
	case DDERR_HEIGHTALIGN:
		errStr = "DDERR_HEIGHTALIGN";
		break;
	case DDERR_INCOMPATIBLEPRIMARY:
		errStr = "DDERR_INCOMPATIBLEPRIMARY";
		break;
	case DDERR_INVALIDCAPS:
		errStr = "DDERR_INVALIDCAPS";
		break;
	case DDERR_INVALIDCLIPLIST:
		errStr = "DDERR_INVALIDCLIPLIST";
		break;
	case DDERR_INVALIDMODE:
		errStr = "DDERR_INVALIDMODE";
		break;
	case DDERR_INVALIDOBJECT:
		errStr = "DDERR_INVALIDOBJECT";
		break;
	case DDERR_INVALIDPARAMS:
		errStr = "DDERR_INVALIDPARAMS";
		break;
	case DDERR_INVALIDPIXELFORMAT:
		errStr = "DDERR_INVALIDPIXELFORMAT";
		break;
	case DDERR_INVALIDRECT:
		errStr = "DDERR_INVALIDRECT";
		break;
	case DDERR_LOCKEDSURFACES:
		errStr = "DDERR_LOCKEDSURFACES";
		break;
	case DDERR_NO3D:
		errStr = "DDERR_NO3D";
		break;
	case DDERR_NOALPHAHW:
		errStr = "DDERR_NOALPHAHW";
		break;
	case DDERR_NOCLIPLIST:
		errStr = "DDERR_NOCLIPLIST";
		break;
	case DDERR_NOCOLORCONVHW:
		errStr = "DDERR_NOCOLORCONVHW";
		break;
	case DDERR_NOCOOPERATIVELEVELSET:
		errStr = "DDERR_NOCOOPERATIVELEVELSET";
		break;
	case DDERR_NOCOLORKEY:
		errStr = "DDERR_NOCOLORKEY";
		break;
	case DDERR_NOCOLORKEYHW:
		errStr = "DDERR_NOCOLORKEYHW";
		break;
	case DDERR_NODIRECTDRAWSUPPORT:
		errStr = "DDERR_NODIRECTDRAWSUPPORT";
		break;
	case DDERR_NOEXCLUSIVEMODE:
		errStr = "DDERR_NOEXCLUSIVEMODE";
		break;
	case DDERR_NOFLIPHW:
		errStr = "DDERR_NOFLIPHW";
		break;
	case DDERR_NOGDI:
		errStr = "DDERR_NOGDI";
		break;
	case DDERR_NOMIRRORHW:
		errStr = "DDERR_NOMIRRORHW";
		break;
	case DDERR_NOTFOUND:
		errStr = "DDERR_NOTFOUND";
		break;
	case DDERR_NOOVERLAYHW:
		errStr = "DDERR_NOOVERLAYHW";
		break;
	case DDERR_NORASTEROPHW:
		errStr = "DDERR_NORASTEROPHW";
		break;
	case DDERR_NOROTATIONHW:
		errStr = "DDERR_NOROTATIONHW";
		break;
	case DDERR_NOSTRETCHHW:
		errStr = "DDERR_NOSTRETCHHW";
		break;
	case DDERR_NOT4BITCOLOR:
		errStr = "DDERR_NOT4BITCOLOR";
		break;
	case DDERR_NOT4BITCOLORINDEX:
		errStr = "DDERR_NOT4BITCOLORINDEX";
		break;
	case DDERR_NOT8BITCOLOR:
		errStr = "DDERR_NOT8BITCOLOR";
		break;
	case DDERR_NOTEXTUREHW:
		errStr = "DDERR_NOTEXTUREHW";
		break;
	case DDERR_NOVSYNCHW:
		errStr = "DDERR_NOVSYNCHW";
		break;
	case DDERR_NOZBUFFERHW:
		errStr = "DDERR_NOZBUFFERHW";
		break;
	case DDERR_NOZOVERLAYHW:
		errStr = "DDERR_NOZOVERLAYHW";
		break;
	case DDERR_OUTOFCAPS:
		errStr = "DDERR_OUTOFCAPS";
		break;
	case DDERR_OUTOFMEMORY:
		errStr = "DDERR_OUTOFMEMORY";
		break;
	case DDERR_OUTOFVIDEOMEMORY:
		errStr = "DDERR_OUTOFVIDEOMEMORY";
		break;
	case DDERR_OVERLAYCANTCLIP:
		errStr = "DDERR_OVERLAYCANTCLIP";
		break;
	case DDERR_OVERLAYCOLORKEYONLYONEACTIVE:
		errStr = "DDERR_OVERLAYCOLORKEYONLYONEACTIVE";
		break;
	case DDERR_PALETTEBUSY:
		errStr = "DDERR_PALETTEBUSY";
		break;
	case DDERR_COLORKEYNOTSET:
		errStr = "DDERR_COLORKEYNOTSET";
		break;
	case DDERR_SURFACEALREADYATTACHED:
		errStr = "DDERR_SURFACEALREADYATTACHED";
		break;
	case DDERR_SURFACEALREADYDEPENDENT:
		errStr = "DDERR_SURFACEALREADYDEPENDENT";
		break;
	case DDERR_SURFACEBUSY:
		errStr = "DDERR_SURFACEBUSY";
		break;
	case DDERR_CANTLOCKSURFACE:
		errStr = "DDERR_CANTLOCKSURFACE";
		break;
	case DDERR_SURFACEISOBSCURED:
		errStr = "DDERR_SURFACEISOBSCURED";
		break;
	case DDERR_SURFACELOST:
		errStr = "DDERR_SURFACELOST";
		break;
	case DDERR_SURFACENOTATTACHED:
		errStr = "DDERR_SURFACENOTATTACHED";
		break;
	case DDERR_TOOBIGHEIGHT:
		errStr = "DDERR_TOOBIGHEIGHT";
		break;
	case DDERR_TOOBIGSIZE:
		errStr = "DDERR_TOOBIGSIZE";
		break;
	case DDERR_TOOBIGWIDTH:
		errStr = "DDERR_TOOBIGWIDTH";
		break;
	case DDERR_UNSUPPORTED:
		errStr = "DDERR_UNSUPPORTED";
		break;
	case DDERR_UNSUPPORTEDFORMAT:
		errStr = "DDERR_UNSUPPORTEDFORMAT";
		break;
	case DDERR_UNSUPPORTEDMASK:
		errStr = "DDERR_UNSUPPORTEDMASK";
		break;
	case DDERR_VERTICALBLANKINPROGRESS:
		errStr = "DDERR_VERTICALBLANKINPROGRESS";
		break;
	case DDERR_WASSTILLDRAWING:
		errStr = "DDERR_WASSTILLDRAWING";
		break;
	case DDERR_XALIGN:
		errStr = "DDERR_XALIGN";
		break;
	case DDERR_INVALIDDIRECTDRAWGUID:
		errStr = "DDERR_INVALIDDIRECTDRAWGUID";
		break;
	case DDERR_DIRECTDRAWALREADYCREATED:
		errStr = "DDERR_DIRECTDRAWALREADYCREATED";
		break;
	case DDERR_NODIRECTDRAWHW:
		errStr = "DDERR_NODIRECTDRAWHW";
		break;
	case DDERR_PRIMARYSURFACEALREADYEXISTS:
		errStr = "DDERR_PRIMARYSURFACEALREADYEXISTS";
		break;
	case DDERR_NOEMULATION:
		errStr = "DDERR_NOEMULATION";
		break;
	case DDERR_REGIONTOOSMALL:
		errStr = "DDERR_REGIONTOOSMALL";
		break;
	case DDERR_CLIPPERISUSINGHWND:
		errStr = "DDERR_CLIPPERISUSINGHWND";
		break;
	case DDERR_NOCLIPPERATTACHED:
		errStr = "DDERR_NOCLIPPERATTACHED";
		break;
	case DDERR_NOHWND:
		errStr = "DDERR_NOHWND";
		break;
	case DDERR_HWNDSUBCLASSED:
		errStr = "DDERR_HWNDSUBCLASSED";
		break;
	case DDERR_HWNDALREADYSET:
		errStr = "DDERR_HWNDALREADYSET";
		break;
	case DDERR_NOPALETTEATTACHED:
		errStr = "DDERR_NOPALETTEATTACHED";
		break;
	case DDERR_NOPALETTEHW:
		errStr = "DDERR_NOPALETTEHW";
		break;
	case DDERR_BLTFASTCANTCLIP:
		errStr = "DDERR_BLTFASTCANTCLIP";
		break;
	case DDERR_NOBLTHW:
		errStr = "DDERR_NOBLTHW";
		break;
	case DDERR_NODDROPSHW:
		errStr = "DDERR_NODDROPSHW";
		break;
	case DDERR_OVERLAYNOTVISIBLE:
		errStr = "DDERR_OVERLAYNOTVISIBLE";
		break;
	case DDERR_NOOVERLAYDEST:
		errStr = "DDERR_NOOVERLAYDEST";
		break;
	case DDERR_INVALIDPOSITION:
		errStr = "DDERR_INVALIDPOSITION";
		break;
	case DDERR_NOTAOVERLAYSURFACE:
		errStr = "DDERR_NOTAOVERLAYSURFACE";
		break;
	case DDERR_EXCLUSIVEMODEALREADYSET:
		errStr = "DDERR_EXCLUSIVEMODEALREADYSET";
		break;
	case DDERR_NOTFLIPPABLE:
		errStr = "DDERR_NOTFLIPPABLE";
		break;
	case DDERR_CANTDUPLICATE:
		errStr = "DDERR_CANTDUPLICATE";
		break;
	case DDERR_NOTLOCKED:
		errStr = "DDERR_NOTLOCKED";
		break;
	case DDERR_CANTCREATEDC:
		errStr = "DDERR_CANTCREATEDC";
		break;
	case DDERR_NODC:
		errStr = "DDERR_NODC";
		break;
	case DDERR_WRONGMODE:
		errStr = "DDERR_WRONGMODE";
		break;
	case DDERR_IMPLICITLYCREATED:
		errStr = "DDERR_IMPLICITLYCREATED";
		break;
	case DDERR_NOTPALETTIZED:
		errStr = "DDERR_NOTPALETTIZED";
		break;
	case DDERR_UNSUPPORTEDMODE:
		errStr = "DDERR_UNSUPPORTEDMODE";
		break;
	case DDERR_NOMIPMAPHW:
		errStr = "DDERR_NOMIPMAPHW";
		break;
	case DDERR_INVALIDSURFACETYPE:
		errStr = "DDERR_INVALIDSURFACETYPE";
		break;
	case DDERR_NOOPTIMIZEHW:
		errStr = "DDERR_NOOPTIMIZEHW";
		break;
	case DDERR_NOTLOADED:
		errStr = "DDERR_NOTLOADED";
		break;
	case DDERR_DCALREADYCREATED:
		errStr = "DDERR_DCALREADYCREATED";
		break;
	case DDERR_NONONLOCALVIDMEM:
		errStr = "DDERR_NONONLOCALVIDMEM";
		break;
	case DDERR_CANTPAGELOCK:
		errStr = "DDERR_CANTPAGELOCK";
		break;
	case DDERR_CANTPAGEUNLOCK:
		errStr = "DDERR_CANTPAGEUNLOCK";
		break;
	case DDERR_NOTPAGELOCKED:
		errStr = "DDERR_NOTPAGELOCKED";
		break;
	case DDERR_MOREDATA:
		errStr = "DDERR_MOREDATA";
		break;
	case DDERR_VIDEONOTACTIVE:
		errStr = "DDERR_VIDEONOTACTIVE";
		break;
	case DDERR_DEVICEDOESNTOWNSURFACE:
		errStr = "DDERR_DEVICEDOESNTOWNSURFACE";
		break;
	case DDERR_NOTINITIALIZED:
		errStr = "DDERR_NOTINITIALIZED";
		break;
	}

	if( errStr )
		sprintf( retStr, "%s (%s)", s, errStr );
	else
		sprintf( retStr, "%s (%x)", s, rc );

	return retStr;
}
