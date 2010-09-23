// Filename    : OLOCBMP.H
// Description : Header file of Location Bitmap cache


#ifndef __OLOCBMP_H
#define __OLOCBMP_H

#define NULL 0

#define RHOMBUS_LOCATION 1

class Blob2DW;

class LocationBitmap
{
public:
	int	init_flag;
	int	top_x_loc;
	int	top_y_loc;
	int	column_count;
	int	row_count;

	int	adj_col;
	int	adj_row;

	Blob2DW **bitmap_array;
	Blob2DW **sec_bitmap_array;

public:
	LocationBitmap();
	~LocationBitmap();

	void	init(int cachedColumn, int cachedRow, int viewColumn, int viewRow);
	void	deinit();

	void	set_top_loc(int xLoc, int yLoc);
	int	is_loc_cached(int xLoc, int yLoc);
	Blob2DW*	get_bitmap(int xLoc, int yLoc);
	Blob2DW*	get_new_bitmap(int xLoc, int yLoc);
	void	scroll_to( int xLoc, int yLoc);


private:
	int	get_row(int xLoc, int yLoc);
	int	get_column(int xLoc, int yLoc);
	int	get_x_loc(int column, int row);
	int	get_y_loc(int row, int column);
};

extern LocationBitmap loc_bitmap_cache;

#endif