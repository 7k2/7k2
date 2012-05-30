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

#include <oblob2.h>
#include <all.h>
#include <colcode.h>
#include <obitmap.h>


// ------- define constant -------//

#define BACKGROUND_COLOR TRANSPARENT_CODE
#define DEFAULT_BLOB2D_SIZE 0x1000
#define DEFAULT_BLOB2D_INC 0x100


Blob2D::Blob2D()
{
	left_edge = top_edge = width = height = 0;
	alloc_size = DEFAULT_BLOB2D_SIZE;
	ptr = (Bitmap *)mem_add(alloc_size);
	ptr->init(width, height);
}

Blob2D::Blob2D( Blob2D &b )
{
	left_edge = b.left_edge;
	top_edge = b.top_edge;
	width = b.width;
	height = b.height;
	alloc_size = b.alloc_size;

	err_when( width < 0 );
	err_when( height < 0 );

	ptr = (Bitmap *)mem_add(alloc_size);
	memcpy(ptr, b.ptr, alloc_size);
	// ((Bitmap *)ptr)->init(width, height);
}


Blob2D::~Blob2D()
{
	if(ptr)
		mem_del(ptr);
}

Blob2D& Blob2D::operator=(Blob2D &b)
{
	left_edge = b.left_edge;
	top_edge = b.top_edge;
	width = b.width;
	height = b.height;

	err_when( width < 0 );
	err_when( height < 0 );

	size_t siz = Bitmap::size(width,height);
	if( alloc_size < siz )
	{
		if(ptr)
			mem_del(ptr);
		alloc_size = b.alloc_size;
		ptr = (Bitmap *)mem_add(alloc_size);
	}
	memcpy(ptr, b.ptr, siz);

	// ((Bitmap *)ptr)->init(width, height);

	return *this;
}


void Blob2D::resize(short destLeft, short destTop, short destWidth, short destHeight)
{
	if( width == 0 && height == 0)
	{
		size_t siz = Bitmap::size(destWidth,destHeight);
		if( alloc_size < siz )
		{
			if(ptr)
				mem_del(ptr);
			ptr = (Bitmap *)mem_add((alloc_size = siz + DEFAULT_BLOB2D_INC));
		}
		memset(ptr, BACKGROUND_COLOR, siz);
		ptr->init(destWidth, destHeight);
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

		size_t siz = Bitmap::size(destWidth,destHeight);
		if( alloc_size < siz )
		{
			ptr = (Bitmap *)mem_resize(ptr, (alloc_size = siz + DEFAULT_BLOB2D_INC));
		}

		if( destHeight > height && destWidth > 0 )
		{
			int y2 = top_edge+height;	// must keep the old instant
			height = destHeight;			// as height must be change to make fill_area correct

			// extend, then fill the new with the background color
			fill_area(destLeft, top_edge+height, destLeft+destWidth-1, destTop+destHeight-1, BACKGROUND_COLOR, 0);
		}
	}
	else if( left_edge <= destLeft && top_edge <= destTop &&
		left_edge+width >= destLeft+destWidth && top_edge+height >= destTop+destHeight)
	{
		// clipping
		unsigned char *src = ptr->get_ptr(destLeft-left_edge, destTop-top_edge);
		int srcPitch = ptr->get_pitch();
		ptr->init(destWidth, destHeight);
		unsigned char *dest = ptr->get_ptr();
		int destPitch = ptr->get_pitch();

		for(int y = 0; y < destHeight; ++y, src += srcPitch, dest += destPitch )
			memmove(dest, src, destWidth);

		// ptr = (Bitmap *)mem_resize(ptr, ptr->size());
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
			size_t siz = Bitmap::size(destWidth, destHeight);
			Bitmap *newPtr = (Bitmap *)mem_add(siz + DEFAULT_BLOB2D_INC);
			memset(newPtr, BACKGROUND_COLOR, siz);
			newPtr->init(destWidth, destHeight);

			if( copyWidth > 0 && copyHeight > 0 )
			{
				int yCount = 0;
				unsigned char *src = ptr->get_ptr(copyLeft-left_edge, yCount+copyTop-top_edge );
				unsigned char *dest = newPtr->get_ptr(copyLeft-destLeft, yCount+copyTop-destTop );

				for( ; yCount < copyHeight; ++yCount, src += ptr->get_pitch(), dest += ptr->get_pitch() )
				{
					// unsigned char *src = (yCount+copyTop-top_edge)*width + copyLeft-left_edge;
					// unsigned char *dest = (yCount+copyTop-destTop)*destWdith + copyLeft-destLeft;
					memcpy(dest, src, copyWidth);
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

int Blob2D::extend_to(short x, short y)
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


int Blob2D::extend_to(short x1, short y1, short x2, short y2)
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

unsigned char *Blob2D::p(short x, short y, int autoExtend)
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


void Blob2D::fill_area(short x1, short y1, short x2, short y2, unsigned char color,
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

	int y = y1;
	unsigned char *dest = ptr->get_ptr( x1 - left_edge, y - top_edge );
	for( ; y <= y2; ++y, dest += width )
		memset(dest, color, x2-x1+1);
}



void Blob2D::auto_clip( short *pLeft, short *pTop, short *pWidth, short *pHeight, int autoResize)
{
	short x1, y1, x2, y2;
	int pitch = ptr->get_pitch();

	y1 = 0;
	y2 = height - 1;
	x1 = 0;
	x2 = width - 1;

	// clip bottom
	for( ; y1 <= y2; --y2)
	{
		int x = x1;
		unsigned char *src = ptr->get_ptr(x, y2);
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
		unsigned char *src = ptr->get_ptr(x, y1);
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
		unsigned char *src = ptr->get_ptr(x1, y);
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
		unsigned char *src = ptr->get_ptr(x1, y);
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
		ptr = (Bitmap *)mem_resize(ptr, (alloc_size = ptr->size()) );
	}
}


void Blob2D::clear()
{
	err_when(!ptr);
	ptr->init(0, 0);

	left_edge = 0;
	top_edge = 0;
	width = 0;
	height = 0;
}

