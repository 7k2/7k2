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

//Filename    : OIMGES.CPP
//Description : Object ImageRes

#include <string.h>

#include <ALL.H>
#include <OVGA.H>
#include <OMOUSE.H>
#include <OIMGRES.H>
#include <OBITMAP.H>


//--------- Format of RES file ------------//
//
// In the resource file, contain many compressed images, each image
// has the following data
//
// <char[8]> = the name of the image
// <int>     = the widht of the image
// <int>     = the height of the image
// <char...> = the bitmap of the image
//
//--------------------------------------------//


//------- Start of function ImageRes::ImageRes -------//
//
// <char*> resName   = name of the resource file (e.g. "GIF.RES")
// [int]   readAll   = whether read all data into the buffer or read one each time
//                     (default:0)
// [int]   useCommonBuf = whether use the sys common buffer to store the data or not
//                     (default:0)
//
ImageRes::ImageRes(char* resFile, int readAll, int useCommonBuf) :
					  ResourceIdx(resFile, readAll, useCommonBuf)
{
}
//--------- End of function ImageRes::ImageRes -------//


//-------- Start of function ImageRes::put_front --------//
//
// int 	x,y       = the location of the image
// char* imageName = name of the image
// [int] compressFlag = compress flag
//								(default: 0)
//
void ImageRes::put_front(int x, int y, char* imageName, int compressFlag)
{
	char* bitmapPtr = ResourceIdx::read(imageName);

	if(!bitmapPtr)
		return;

	mouse.hide_area( x, y, x+*((short*)bitmapPtr)-1, y+*(((short*)bitmapPtr)+1)-1 );

	if( compressFlag )
		vga_front.put_bitmap_trans_decompress( x, y, bitmapPtr );
	else
		vga_front.put_bitmap_trans( x, y, bitmapPtr );

	mouse.show_area();
}
//---------- End of function ImageRes::put_front --------//


//-------- Start of function ImageRes::put_back --------//
//
// int 	x,y       = the location of the image
// char* imageName = name of the image
// [int] compressFlag = compress flag
//								(default: 0)
//
void ImageRes::put_back(int x, int y, char* imageName, int compressFlag)
{
	char* bitmapPtr = ResourceIdx::read(imageName);

	if( bitmapPtr )
	{
		if( compressFlag )
			vga_back.put_bitmap_trans_decompress( x, y, bitmapPtr );
		else
			vga_back.put_bitmap_trans( x, y, bitmapPtr );
	}
}
//---------- End of function ImageRes::put_back --------//


//-------- Start of function ImageRes::put_front --------//
//
// int x,y      = the location of the image
// int bitmapId = id. of the bitmap
// [int] compressFlag = compress flag
//								(default: 0)
//
void ImageRes::put_front(int x, int y, int bitmapId, int compressFlag)
{
	char* bitmapPtr = ResourceIdx::get_data(bitmapId);

	if( !bitmapPtr )
		return;

	mouse.hide_area( x, y, x+*((short*)bitmapPtr)-1, y+*(((short*)bitmapPtr)+1)-1 );

	if( compressFlag )
		vga_front.put_bitmap_trans_decompress( x, y, bitmapPtr );
	else
		vga_front.put_bitmap_trans( x, y, bitmapPtr );

	mouse.show_area();
}
//---------- End of function ImageRes::put_front --------//


//-------- Start of function ImageRes::put_back --------//
//
// int ,y       = the location of the image
// int bitmapId = id. of the bitmap
// [int] compressFlag = compress flag
//								(default: 0)
//
void ImageRes::put_back(int x, int y, int bitmapId, int compressFlag)
{
	char* bitmapPtr = ResourceIdx::get_data(bitmapId);

	if( bitmapPtr )
	{
		if( compressFlag )
			vga_back.put_bitmap_trans_decompress( x, y, bitmapPtr );
		else
			vga_back.put_bitmap_trans( x, y, bitmapPtr );
	}
}
//---------- End of function ImageRes::put_back --------//


//-------- Start of function ImageRes::put_join --------//
//
// int 	x,y       = the location of the image
// char* imageName = name of the image
//
void ImageRes::put_join(int x, int y, char* imageName)
{
	char* bitmapPtr = ResourceIdx::read(imageName);

	if( !bitmapPtr )
		return;

	mouse.hide_area( x, y, x+((Bitmap *)bitmapPtr)->get_width()-1, y+((Bitmap *)bitmapPtr)->get_height()-1 );

	if( bitmapPtr )
		vga_front.join_trans( &vga_back, x, y, bitmapPtr );

	mouse.show_area();
}
//---------- End of function ImageRes::put_join --------//


//-------- Start of function ImageRes::put_large --------//
//
// When a picture file is > 64K, which cannot be read into a single
// memory buffer.
//
// It will call vga.put_pict() which will continously read the file
// and put to the screen until completion.
//
// <VgaBuf*> vgaBuf 	  = the vga buffer for display
// <int>	    x,y       = the location of the image
// <char*>   imageName = name of the image
//
void ImageRes::put_large(VgaBuf* vgaBuf, int x, int y, char* imageName)
{
	int dataSize;

	vgaBuf->put_large_bitmap( x, y, ResourceIdx::get_file(imageName, dataSize) );
}
//---------- End of function ImageRes::put_large --------//


//-------- Start of function ImageRes::put_large --------//
//
// When a picture file is > 64K, which cannot be read into a single
// memory buffer.
//
// It will call vga.put_pict() which will continously read the file
// and put to the screen until completion.
//
// <VgaBuf*> vgaBuf 	  = the vga buffer for display
// <int>	    x,y       = the location of the image
// <int>		 bitmapId  = id. of the bitmap in the bitmap resource file.
//
void ImageRes::put_large(VgaBuf* vgaBuf, int x, int y, int bitmapId)
{
	int dataSize;

	vgaBuf->put_large_bitmap( x, y, ResourceIdx::get_file(bitmapId, dataSize) );
}
//---------- End of function ImageRes::put_large --------//

//-------- Start of function ImageRes::put_to_buf --------//
//
// Put the image to the specified Vga buffer. 
//
// <VgaBuf*> vgaBufPtr = the pointer to the Vga buffer
// <char*>	 imageName = name of the image
//
void ImageRes::put_to_buf(VgaBuf* vgaBufPtr, char* imageName)
{
	if( read_all )
	{
		vgaBufPtr->put_bitmap(0, 0, get_ptr(imageName));
	}
	else
	{
		int	dataSize;
		File* filePtr = get_file(imageName, dataSize);
		vgaBufPtr->put_large_bitmap(0, 0, filePtr);
	}
}
//---------- End of function ImageRes::put_to_buf --------//


//-------- Start of function ImageRes::put_to_buf --------//
//
// Put the image to the specified Vga buffer.
//
// <VgaBuf*> vgaBufPtr = the pointer to the Vga buffer
// <int>     bitmapId  = id. of the bitmap in the resource file.
//
void ImageRes::put_to_buf(VgaBuf* vgaBufPtr, int bitmapId)
{
	if( read_all )
	{
		vgaBufPtr->put_bitmap(0, 0, get_data(bitmapId));
	}
	else
	{
		int	dataSize;
		File* filePtr = get_file(bitmapId, dataSize);
		vgaBufPtr->put_large_bitmap(0, 0, filePtr);
	}
}
//---------- End of function ImageRes::put_to_buf --------//
