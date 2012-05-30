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

// Filename    : OBLOB2.CPP
// Description : 2D binary block, four side extensible

#include <oblob2w.h>
#include <all.h>
#include <colcode.h>
#include <imgfun.h>


// ------- define constant -------//

#define BACKGROUND_COLOR transparent_code_w
#define DEFAULT_BLOB2DW_SIZE 0x2000
#define DEFAULT_BLOB2DW_INC 0x200


Blob2DW::Blob2DW()
{
	left_edge = top_edge = width = height = 0;
	alloc_size = DEFAULT_BLOB2DW_SIZE;
	ptr = (BitmapW *)mem_add(alloc_size);
	ptr->init(width, height);
}

Blob2DW::Blob2DW( Blob2DW &b )
{
	left_edge = b.left_edge;
	top_edge = b.top_edge;
	width = b.width;
	height = b.height;
	alloc_size = b.alloc_size;

	err_when( width < 0 );
	err_when( height < 0 );

	ptr = (BitmapW *)mem_add(alloc_size);
	memcpy(ptr, b.ptr, alloc_size);
	// ((BitmapW *)ptr)->init(width, height);
}


Blob2DW::~Blob2DW()
{
	if(ptr)
		mem_del(ptr);
}

Blob2DW& Blob2DW::operator=(Blob2DW &b)
{
	left_edge = b.left_edge;
	top_edge = b.top_edge;
	width = b.width;
	height = b.height;

	err_when( width < 0 );
	err_when( height < 0 );

	size_t siz = BitmapW::size(width,height);
	if( alloc_size < siz )
	{
		if(ptr)
			mem_del(ptr);
		alloc_size = b.alloc_size;
		ptr = (BitmapW *)mem_add(alloc_size);
	}
	memcpy(ptr, b.ptr, siz);

	// ((BitmapW *)ptr)->init(width, height);

	return *this;
}


void Blob2DW::resize(short destLeft, short destTop, short destWidth, short destHeight)
{
	if( width == 0 && height == 0)
	{
		size_t siz = BitmapW::size(destWidth,destHeight);
		if( alloc_size < siz )
		{
			if(ptr)
				mem_del(ptr);
			ptr = (BitmapW *)mem_add((alloc_size = siz + DEFAULT_BLOB2DW_INC));
		}
		ptr->init(destWidth, destHeight);
		IMGbar(ptr->get_ptr(), ptr->get_true_pitch(), 0, 0, destWidth-1, destHeight-1, BACKGROUND_COLOR);
	}
	else if( destWidth == 0 && destHeight == 0 )
	{
		err_when(!ptr);
		ptr->init(destWidth, destHeight);
	}
	else if( left_edge == destLeft && top_edge == destTop && width == destWidth )
	{
		if( destHeight == height )
			return;		// unchange

		size_t siz = BitmapW::size(destWidth,destHeight);
		if( alloc_size < siz )
		{
			ptr = (BitmapW *)mem_resize(ptr, (alloc_size = siz + DEFAULT_BLOB2DW_INC));
		}

		if( destHeight > height && destWidth > 0 )
		{
			int y2 = top_edge+height;	// must keep the old instant
			height = destHeight;			// as height must be change to make fill_area correct

			// extend, then fill the new with the background color
			// ##### begin Gilbert 14/10 #######//
			fill_area(destLeft, y2, destLeft+destWidth-1, destTop+destHeight-1, BACKGROUND_COLOR, 0);
			// ##### end Gilbert 14/10 #######//
		}
	}
	else if( left_edge <= destLeft && top_edge <= destTop &&
		left_edge+width >= destLeft+destWidth && top_edge+height >= destTop+destHeight)
	{
		// clipping
		short *src = ptr->get_ptr(destLeft-left_edge, destTop-top_edge);
		int srcPitch = ptr->get_pitch();
		ptr->init(destWidth, destHeight);
		short *dest = ptr->get_ptr();
		int destPitch = ptr->get_pitch();

		for(int y = 0; y < destHeight; ++y, src += srcPitch, dest += destPitch )
			memmove(dest, src, destWidth * sizeof(short));

		// ptr = (BitmapW *)mem_resize(ptr, ptr->size());
	}
	else
	{
		// general resize, new another buffer
		// copy range, intersection of two area :
		short copyLeft, copyTop, copyWidth, copyHeight;
		copyLeft = MAX(destLeft, left_edge);
		copyTop = MAX(destTop, top_edge);
		copyWidth = MIN(destLeft + destWidth, left_edge + width) - copyLeft;
		copyHeight = MIN(destTop + destHeight, top_edge + height) - copyTop;

		{
			size_t siz = BitmapW::size(destWidth, destHeight);
			BitmapW *newPtr = (BitmapW *)mem_add(siz + DEFAULT_BLOB2DW_INC);
			newPtr->init(destWidth, destHeight);
			IMGbar(newPtr->get_ptr(), newPtr->get_true_pitch(), 0, 0, destWidth-1, destHeight-1, BACKGROUND_COLOR);

			if( copyWidth > 0 && copyHeight > 0 )
			{
				int yCount = 0;
				short *src = ptr->get_ptr(copyLeft-left_edge, yCount+copyTop-top_edge );
				short *dest = newPtr->get_ptr(copyLeft-destLeft, yCount+copyTop-destTop );

				for( ; yCount < copyHeight; ++yCount, src += ptr->get_pitch(), dest += ptr->get_pitch() )
				{
					// unsigned char *src = (yCount+copyTop-top_edge)*width + copyLeft-left_edge;
					// unsigned char *dest = (yCount+copyTop-destTop)*destWdith + copyLeft-destLeft;
					memcpy(dest, src, copyWidth * sizeof(short));
				}
			}

			// assign to the newPtr now
			left_edge = destLeft;
			top_edge = destTop;
			width = destWidth;
			height = destHeight;
			if(ptr)
				mem_del(ptr);
			ptr = newPtr;
		}

		// fill rest area with background color
		if( top_edge < copyTop && width > 0)
		{
			fill_area(left_edge, top_edge, left_edge+width-1, copyTop, BACKGROUND_COLOR, 0 );
		}

		// fill bottom
		if( top_edge+height > copyTop+copyHeight && width > 0)
		{
			fill_area(left_edge, copyTop+copyHeight, left_edge+width-1, top_edge+height-1, BACKGROUND_COLOR, 0 );
		}

		// fill left
		if( left_edge < copyLeft && destHeight > 0)
		{
			fill_area(left_edge, copyTop, copyLeft-1, copyTop+copyHeight-1,
				BACKGROUND_COLOR, 0);
		}

		// fill right
		if( left_edge+width > copyLeft+copyWidth && destHeight > 0 )
		{
			fill_area(copyLeft+copyWidth, copyTop, left_edge+width, copyTop+copyHeight-1,
				BACKGROUND_COLOR, 0);
		}
	}

	left_edge = destLeft;
	top_edge = destTop;
	width = destWidth;
	height = destHeight;
}

int Blob2DW::extend_to(short x, short y)
{
	int resizeFlag = 0;
	short newLeft = left_edge;
	short newTop = top_edge;
	short newWidth = width;
	short newHeight = height;

	if( x < left_edge )
	{
		newLeft = x;
		newWidth = left_edge + width - newLeft;
		resizeFlag = 1;
	}
	if( x >= left_edge+width )
	{
		newWidth = x - newLeft + 1;
		resizeFlag = 1;
	}
	if( y < top_edge )
	{
		newTop = y;
		newHeight = top_edge + height - newTop;
		resizeFlag = 1;
	}
	if( y >= top_edge+height )
	{
		newHeight = y - newTop + 1;
		resizeFlag = 1;
	}

	if( resizeFlag )
	{
		resize( newLeft, newTop, newWidth, newHeight );
	}

	return resizeFlag;
}


int Blob2DW::extend_to(short x1, short y1, short x2, short y2)
{
	int resizeFlag = 0;
	short newLeft = left_edge;
	short newTop = top_edge;
	short newWidth = width;
	short newHeight = height;

	if( x1 < left_edge )
	{
		newLeft = x1;
		newWidth = left_edge + width - newLeft;
		resizeFlag = 1;
	}
	if( x2 >= left_edge+width )
	{
		newWidth = x2 - newLeft + 1;
		resizeFlag = 1;
	}
	if( y1 < top_edge )
	{
		newTop = y1;
		newHeight = top_edge + height - newTop;
		resizeFlag = 1;
	}
	if( y2 >= top_edge+height )
	{
		newHeight = y2 - newTop + 1;
		resizeFlag = 1;
	}

	if( resizeFlag )
	{
		resize( newLeft, newTop, newWidth, newHeight );
	}

	return resizeFlag;
}

short *Blob2DW::p(short x, short y, int autoExtend)
{
	if( autoExtend )
	{
		extend_to(x, y);
	}
	else
	{
		if( x < left_edge || x >= left_edge+width
			|| y < top_edge || y >= top_edge+height )
			return NULL;
	}

	return ptr->get_ptr(x-left_edge, y-top_edge);
}


void Blob2DW::fill_area(short x1, short y1, short x2, short y2, short color,
	int autoExtend)
{
	if( autoExtend )
	{
		extend_to(x1, y1, x2, y2);
	}
	else
	{
		// adjust x1, y1, x2 ,y2
		if( x1 < left_edge )
			x1 = left_edge;
		if( x2 >= left_edge+width )
			x2 = left_edge+width-1;
		if( y1 < top_edge )
			y1 = top_edge;
		if( y2 >= top_edge+height )
			y2 = top_edge+height-1;

		if( x2 < x1 || y2 < y1 )
			return;
	}

	IMGbar(ptr->get_ptr(), ptr->get_true_pitch(), x1 - left_edge, y1 - top_edge, 
		x2 - left_edge, y2 - top_edge, color);
// int y = y1;
//	short *dest = ptr->get_ptr( x1 - left_edge, y - top_edge );
//	for( ; y <= y2; ++y, dest += width )
//		memset(dest, color, x2-x1+1);
}



void Blob2DW::auto_clip( short *pLeft, short *pTop, short *pWidth, short *pHeight, int autoResize)
{
	short x1, y1, x2, y2;

	y1 = 0;
	y2 = height - 1;
	x1 = 0;
	x2 = width - 1;

	int pitch = ptr->get_pitch();

	// clip bottom
	for( ; y1 <= y2; --y2)
	{
		int x = x1;
		short *src = ptr->get_ptr(x, y2);
		for( ; x <= x2; ++x, ++src )
		{
			if( *src != BACKGROUND_COLOR )
				break;		// dirty
		}
		if( x <= x2 )
			break;			// dirty
	}	// y2 is now the bottom margin

	// clip top
	for( ; y1 <= y2; ++y1 )
	{
		int x = x1;
		short *src = ptr->get_ptr(x, y1);
		for( ; x <= x2; ++x, ++src )
		{
			if( *src != BACKGROUND_COLOR )
				break;		// dirty
		}
		if( x <= x2 )
			break;			// dirty
	}	// y1 is now the top margin

	// clip right
	for( ; x1 <= x2; --x2)
	{
		int y = y1;
		short *src = ptr->get_ptr(x1, y);
		for( ; y <= y2; ++y, src+=pitch )
		{
			if( *src != BACKGROUND_COLOR )
				break;		// dirty
		}
		if( y <= y2 )
			break;			// dirty
	}	// x2 is now the right margin

	// clip left
	for( ; x1 <= x2; ++x1 )
	{
		int y = y1;
		short *src = ptr->get_ptr(x1, y);
		for( ; y <= y2; ++y, src+=pitch )
		{
			if( *src != BACKGROUND_COLOR )
				break;		// dirty
		}
		if( y <= y2 )
			break;			// dirty
	}	// x1 is now the left margin

	if( pLeft )
		*pLeft = x1 + left_edge;
	if( pTop )
		*pTop = y1 + top_edge;
	if( pWidth )
		*pWidth = x2 - x1 + 1;
	if( pHeight )
		*pHeight = y2 - y1 + 1;

	if( autoResize )
	{
		resize(x1+left_edge, y1+top_edge, x2-x1+1, y2-y1+1);
		ptr = (BitmapW *)mem_resize(ptr, (alloc_size = ptr->size()) );
	}
}


void Blob2DW::clear()
{
	err_when(!ptr);
	ptr->init(0, 0);

	left_edge = 0;
	top_edge = 0;
	width = 0;
	height = 0;
}

