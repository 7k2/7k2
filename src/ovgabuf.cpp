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
#include <dbglog.h>

DBGLOG_DEFAULT_CHANNEL(VgaBuf);

short *VgaBuf::default_remap_table;
short *VgaBuf::default_blend_table;

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


//------ Begin of function VgaBuf::init --------//

void VgaBuf::init(Surface *s, char front)
{
	surface = s;
	is_front = front;
}
//-------- End of function VgaBuf::init ----------//


//------ Begin of function VgaBuf::deinit --------//

void VgaBuf::deinit()
{
	if( surface )
	{
		delete surface;
		surface = NULL;
	}
}
//-------- End of function VgaBuf::deinit ----------//


//------------- Begin of function VgaBuf::lock_buf --------------//

void VgaBuf::lock_buf()
{
	err_if( buf_locked )
		err_now( "VgaBuf::lock_buf() error, buffer already locked." );

	cur_buf_ptr = buf_ptr();
	cur_pitch = buf_true_pitch();

	if( surface->lock_buf() )
		buf_locked = TRUE;
	else
	{
		if( is_front )
			err_now( "VgaBuf::lock_buf() locking front buffer failed." );
		else
			err_now( "VgaBuf::lock_buf() locking back buffer failed." );
	}
}
//--------------- End of function VgaBuf::lock_buf --------------//


//------------- Begin of function VgaBuf::unlock_buf --------------//

void VgaBuf::unlock_buf()
{
	// ####### begin Gilbert 16/9 #####//
	if( !surface )
		return;
	// ####### end Gilbert 16/9 #####//
	err_when( !buf_locked );

	if( surface->unlock_buf() )
		buf_locked = FALSE;
	else
	{
		if( is_front )
			err_now( "VgaBuf::unlock_buf() unlocking front buffer failed." );
		else
			err_now( "VgaBuf::unlock_buf() unlocking back buffer failed." );
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
