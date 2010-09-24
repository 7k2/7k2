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

// Filename    : OLOCBMP.CPP
// Description : Location Bitmap cache

#include <OLOCBMP.H>
#include <ALL.H>
#include <OBLOB2W.H>


// if isometric view, #define RHOMBUS_LOCATION 1
// bitmap_array is halfly utilized, a cell is used only if x+y is even.

LocationBitmap::LocationBitmap()
{
	init_flag = 0;
}

LocationBitmap::~LocationBitmap()
{
	deinit();
}


void LocationBitmap::init(int cachedColumn, int cachedRow, int viewColumn, int viewRow )
{
	deinit();

	row_count = cachedRow;
	column_count = cachedColumn;
	top_x_loc = 0;
	top_y_loc = 0;

	bitmap_array = (Blob2DW **) mem_add(sizeof(Blob2DW *) * cachedRow * cachedColumn);
	memset(bitmap_array, 0 , sizeof(Blob2DW *) * cachedRow * cachedColumn);
	sec_bitmap_array = (Blob2DW **) mem_add(sizeof(Blob2DW *) * cachedRow * cachedColumn);
	memset(sec_bitmap_array, 0 , sizeof(Blob2DW *) * cachedRow * cachedColumn);
	init_flag = 1;


	// if viewColumn > cachedColumn or viewRow > cachedRow, the cache is too small
	// increase cachedColumn and cachedRow

	err_when( viewColumn > cachedColumn );
	err_when( viewRow > cachedRow );

	adj_col = (cachedColumn - viewColumn) / 2;
	adj_row = cachedRow - viewRow > 2 ? 2 : 0;	// bias all extra rows to the bottom side

#ifdef DEBUG
	// test get_row, get_column, get_x_loc, get_y_loc reversibility
	int xLoc, yLoc;
	for( yLoc = -10; yLoc < 20; ++yLoc )
	{
		for( xLoc = -10; xLoc < 20; ++xLoc )
		{
			int r = get_row(xLoc, yLoc);
			int c = get_column(xLoc, yLoc);
			err_when( xLoc != get_x_loc(c, r));
			err_when( yLoc != get_y_loc(c, r));
		}
	}
#endif
}

void LocationBitmap::deinit()
{
	if( init_flag )
	{
		int i;
		for( i = row_count * column_count -1; i >= 0; --i )
		{
			delete bitmap_array[i];
		}

		mem_del(bitmap_array);
		mem_del(sec_bitmap_array);
		init_flag = 0;
	}
}


int LocationBitmap::get_column(int xLoc, int yLoc)
{
#if(RHOMBUS_LOCATION==1)
	return (xLoc - top_x_loc) - (yLoc - top_y_loc);
#else
	return xLoc - top_x_loc;
#endif
}


int LocationBitmap::get_row(int xLoc, int yLoc)
{
#if(RHOMBUS_LOCATION==1)
	return (xLoc - top_x_loc) + (yLoc - top_y_loc);
#else
	return yLoc - top_y_loc;
#endif
}


int LocationBitmap::get_x_loc(int column, int row)
{
#if(RHOMBUS_LOCATION==1)
	return ((column + row) >> 1) + top_x_loc;
	// use >> 1 instead of / 2 because -1 >> 1 return -1, but -1/2 returns 0
#else
	return column + top_x_loc;
#endif
}

int LocationBitmap::get_y_loc(int column, int row)
{
#if(RHOMBUS_LOCATION==1)
	return ((row - column) >> 1) + top_y_loc;
	// use >> 1 instead of / 2 because -1 >> 1 return -1, but -1/2 returns 0
#else
	return row + top_y_loc;
#endif
}


void LocationBitmap::set_top_loc(int xLoc, int yLoc)
{
	if( !init_flag )
		return;

	int colShift = -get_column(xLoc, yLoc);
	int rowShift = -get_row(xLoc, yLoc);

	if( colShift == 0 && rowShift == 0 )
	{
		// do nothing
	}
	else if( colShift >= column_count || -colShift >= column_count ||
		rowShift >= row_count || -rowShift >= row_count )
	{
		// scroll too far, don't scroll, simply clear 
		// clear every cell
		for( int r = 0; r != row_count; r++ )
		{
			int c = 0;
			int destIdx = r*column_count+c;
			for( ; c != column_count; ++c, ++destIdx )
			{
				Blob2DW *blobPtr = bitmap_array[destIdx];
				if( blobPtr )
					blobPtr->clear();
			}
		}
	}
	else
	{
		for( int r = 0; r != row_count; r++ )
		{
			int c = 0;
			int destR = r + rowShift;
			// use % to mod may get incorrect result on negative number
			int rowClearFlag = 0;
			while( destR < 0 )
			{
				destR += row_count;
				rowClearFlag = 1;
			}
			while( destR >= row_count )
			{
				destR -= row_count;
				rowClearFlag = 1;
			}

			for( ; c != column_count; ++c )
			{
				int destC = c + colShift;
				int colClearFlag = 0;
				while( destC < 0 )
				{
					destC += column_count;
					colClearFlag = 1;
				}
				while( destC >= column_count )
				{
					destC -= column_count;
					colClearFlag = 1;
				}

				Blob2DW* blobPtr= bitmap_array[r * column_count + c];
				sec_bitmap_array[destR * column_count + destC] = blobPtr;

				if( blobPtr && (rowClearFlag || colClearFlag) )
					blobPtr->clear();
			}
		}

		// exchange two buffer
		Blob2DW **tempBitmapArray = bitmap_array;
		bitmap_array = sec_bitmap_array;
		sec_bitmap_array = tempBitmapArray;
	}

	top_x_loc = xLoc;
	top_y_loc = yLoc;
}


// if the location is inside the cache area, return true
int LocationBitmap::is_loc_cached(int xLoc, int yLoc)
{
	int r = get_row(xLoc, yLoc);
	int c = get_column(xLoc, yLoc);
	if( c >= 0 && c < column_count && r >= 0 && r < row_count )
		return 1;
	else
		return 0;
}



// if the location is outside the cache area, or inside but
// not fill with the content, return NULL
Blob2DW* LocationBitmap::get_bitmap(int xLoc, int yLoc)
{
	int r = get_row(xLoc, yLoc);
	int c = get_column(xLoc, yLoc);
	if( c >= 0 && c < column_count && r >= 0 && r < row_count )
	{
		return bitmap_array[r*column_count+c];		// it may be NULL
	}
	else
		return NULL;
}


Blob2DW* LocationBitmap::get_new_bitmap(int xLoc, int yLoc)
{
	int r = get_row(xLoc, yLoc);
	int c = get_column(xLoc, yLoc);
	if( c >= 0 && c < column_count && r >= 0 && r < row_count )
	{
		Blob2DW **bitmapPtr = &bitmap_array[r*column_count+c];		// it may be NULL
		if( ! *bitmapPtr )
			*bitmapPtr = new Blob2DW;
		return *bitmapPtr;
	}
	else
		return NULL;
}

// during displaying
// first see if it is cached, get_bitmap()
// if it non-nul , display the Blob2DW pointer
// if it is null, call get_new_bitmap
// if get_new_bitmap still return NULL, create a temporary Blob2DW to store the bitmap of that location
// otherwise fill that Blob2DW



// automatically determine the suitable xLoc, yLoc
void LocationBitmap::scroll_to( int xLoc, int yLoc)
{
	// pass world.zoom_matrix->top_x_loc, top_y_loc as xLoc, yLoc

	int c = get_column(xLoc, yLoc) - adj_col;
	int r = get_row(xLoc, yLoc) - adj_row;

	set_top_loc(get_x_loc(c,r), get_y_loc(c,r));
}
