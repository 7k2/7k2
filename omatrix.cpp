//Filename    : OMATRIX.CPP
//Description : Object road direction turn, derived by World, Chain class

#include <ALL.H>
#include <OVGA.H>
#include <OMOUSE.H>
#include <OSYS.H>
#include <OSITE.H>
#include <OFIRM.H>
#include <OTERRAIN.H>
#include <OPLANT.H>
#include <OPOWER.H>
#include <OUNIT.H>
#include <OWORLD.H>
#include <OHILLRES.H>
#include <OBITMAPW.H>

//----------- Begin of function Matrix::init ----------//
//
// <int> winX1,winY1 = the coordination of the win,
//       winX2,winY2   including the scroll bar and up,down panel
//
// <int> areaWidth   = width and height of the bitmap area
//       areaHeight
//
// <int> locWidth    = width and height of each location
// <int> locHeight
//
// <int> maxCargoWidth  = max. widht of height of cargo
//       maxCargoHeight  (unit is Loc, not pixel
// <float> locXWidth = screen width of location x
// <float> locXHeight = screen height of location x
// <float> locYWidth = screen width of location x
// <float> locYHeight = screen height of location y
//
// <int> saveAreaToBuf = whether save the area to a buffer,
//                       set it true - when the area will be ruined by
//                                     animation
//
void Matrix::init(int winX1, int winY1, int winX2, int winY2, int areaWidth, int areaHeight, 
	float locXWidth, float locXHeight, float locYWidth, float locYHeight, 
	char dispRecType,
	int saveAreaToBuf )
{
	// BUGHERE : MAX_BRIGHTNESS_ADJUST_DEGREE changed from 10 to 32
	err_when(FULL_VISIBILITY != MAX_BRIGHTNESS_ADJUST_DEGREE * 4 + 3);
	win_x1 = winX1;      // window area,
	win_y1 = winY1;      // including scroll bar
	win_x2 = winX2;
	win_y2 = winY2;

	image_width  = areaWidth;
	image_height = areaHeight;

	image_x1 = win_x1;  // bitmap area only
	image_y1 = win_y1;
	image_x2 = image_x1+image_width-1;
	image_y2 = image_y1+image_height-1;

	if( locXWidth >= 0.0 )
		loc_x_width = int( locXWidth * (1 << LOC_XY_MUL_SHIFT));
	else
		loc_x_width = - int( -locXWidth * (1 << LOC_XY_MUL_SHIFT) );
	if( locXHeight >= 0.0 )
		loc_x_height = int( locXHeight * (1 << LOC_XY_MUL_SHIFT));
	else
		loc_x_height = - int( -locXHeight * (1 << LOC_XY_MUL_SHIFT) );

	if( locYWidth >= 0.0 )
		loc_y_width = int( locYWidth * (1 << LOC_XY_MUL_SHIFT));
	else
		loc_y_width = - int( -locYWidth * (1 << LOC_XY_MUL_SHIFT) );
	if( locYHeight >= 0.0 )
		loc_y_height = int( locYHeight * (1 << LOC_XY_MUL_SHIFT));
	else
		loc_y_height = - int( -locYHeight * (1 << LOC_XY_MUL_SHIFT) );

	int determinant = loc_x_width * loc_y_height - loc_x_height * loc_y_width;
	int shiftTmp;
	for( shiftTmp = determinant, determinant_shift = 0; shiftTmp > 1; ++determinant_shift, shiftTmp >>= 1);
	err_when( 1 << determinant_shift != determinant );

	//----------------------------------------------//

	if( saveAreaToBuf )
	{
		// ##### begin Gilbert 20/10 ######//
		save_image_buf = (short *)BitmapW::mem_add_bitmap(image_width, image_height);
		// ##### end Gilbert 20/10 ######//
	}
	else
		save_image_buf = NULL;

   just_drawn_flag = 0;

	//---------------------------------------------//

	loc_matrix = NULL;
	corner_matrix = NULL;

	top_x_loc = 0;
	top_y_loc = 0;

	cur_x_loc = -1;
	cur_y_loc = -1;

	cur_cargo_rect_type = RHOMBUS_LOCATION;
	disp_rect_type = dispRecType;

}
//------------- End of function Matrix::init -----------//


//----------- Begin of function Matrix::~Matrix ----------//

Matrix::~Matrix()
{
	if( save_image_buf )
		mem_del( save_image_buf );

	if( own_matrix && loc_matrix )       // the matrix is allocated by us
		mem_del( loc_matrix );

	if( own_matrix && corner_matrix )       // the matrix is allocated by us
		mem_del( corner_matrix );
}
//------------- End of function Matrix::~Matrix -----------//


//----------- Begin of function Matrix::assign_map -----------//
//
// Instead of loading the map file, we can assign a pre-loaded
// map to it.
//
void Matrix::assign_map(Matrix* matrixPtr)
{
	own_matrix = 0;

	loc_matrix = matrixPtr->loc_matrix;
	corner_matrix = matrixPtr->corner_matrix;

	max_x_loc  = matrixPtr->max_x_loc;
	max_y_loc  = matrixPtr->max_y_loc;

	init_var();

	top_x_loc  = 0;
	top_y_loc  = 0;

	cur_x_loc = 0;
   cur_y_loc = 0;
}
//------------- End of function Matrix::assign_map -----------//


//----------- Begin of function Matrix::assign_map -----------//
//
// Instead of loading the map file, we can assign a pre-loaded
// map to it.
//
void Matrix::assign_map(Location* locMatrix, int maxXLoc, int maxYLoc, Corner *cornerMatrix )
{
	own_matrix = 0;

   loc_matrix = locMatrix;
	corner_matrix = cornerMatrix;

   max_x_loc  = maxXLoc;
   max_y_loc  = maxYLoc;

   init_var();

   cur_x_loc = 0;
	cur_y_loc = 0;
}
//------------- End of function Matrix::assign_map -----------//


//-------- Begin of function Matrix::init_var ---------//
//
// Called by assign_map() & MatrixMap::set_view_map()
//
void Matrix::init_var()
{
	if( !disp_rect_type )
	{
		disp_x_loc = (image_width << LOC_XY_MUL_SHIFT)/loc_x_width;		// loc_x_width is multiplied by LOC_XY_MUL_SHIFT
		center_x = disp_x_loc / 2;
		if( disp_x_loc > max_x_loc )
			disp_x_loc = max_x_loc;
		if( center_x > max_x_loc )
			center_x = max_x_loc;
		disp_y_loc = (image_height << LOC_XY_MUL_SHIFT)/loc_y_height;  // loc_x_width is multiplied by LOC_XY_MUL_SHIFT
		center_y = disp_y_loc / 2;
		if( disp_y_loc > max_y_loc )
			disp_y_loc = max_y_loc;
		if( center_y > max_y_loc )
			center_y = max_y_loc;
	}
	else
	{
		disp_x_loc = (image_width << (LOC_XY_MUL_SHIFT+1)) / (loc_x_width - loc_y_width);
		// #### begin Gilbert 1/2 ########//
		// disp_y_loc = (image_width << (LOC_XY_MUL_SHIFT+1)) / (loc_x_height + loc_y_height);
		disp_y_loc = (image_height << (LOC_XY_MUL_SHIFT+1)) / (loc_x_height + loc_y_height);
		// #### end Gilbert 1/2 ########//
		center_x = get_x_loc( disp_x_loc / 2, disp_y_loc / 2 );
		center_y = get_y_loc( disp_x_loc / 2, disp_y_loc / 2 );
	}

	// set top_x_loc, top_y_loc
	top_x_loc = max_x_loc/2 - center_x;
	top_y_loc = max_y_loc/2 - center_y;
}
//------------ End of function Matrix::init_var -----------//


//----------- Begin of function Matrix::paint ------------//
//
// Paint world window and scroll bars
//
void Matrix::paint()
{
}
//----------- End of function Matrix::paint ------------//


//---------- Begin of function Matrix::draw ------------//
//
// Draw world zoom
//
void Matrix::draw()
{
	int       i=0, x, y, xLoc, yLoc;
	if( !disp_rect_type )
	{
		Location* locPtr;
		int maxXLoc = top_x_loc + disp_x_loc;        // divide by 2 for world_info
		int maxYLoc = top_y_loc + disp_y_loc;
		for( y=image_y1,yLoc=top_y_loc ; yLoc<maxYLoc ; yLoc++, y+=loc_y_height )
		{
			if( (i++)%10==0 )
				sys.yield();

			locPtr = get_loc(top_x_loc,yLoc);

			for( x=image_x1,xLoc=top_x_loc ; xLoc<maxXLoc ; xLoc++, x+=loc_x_width, locPtr++ )
				draw_loc( x, y, xLoc, yLoc, locPtr );
		}
	}
	else
	{
		int c0 = get_column(top_x_loc, top_y_loc);
		int r0 = get_row(top_x_loc, top_y_loc);
		int maxCol = c0 + disp_x_loc;
		int maxRow = r0 + disp_y_loc;
		int c, r;
		int odd = 0;
		for( r = r0; r < maxRow; r += 1, odd = !odd )
		{
			if( (i++)%10==0 )
				sys.yield();

			c = c0 + odd;			// odd line start at column 1
			xLoc = get_x_loc( c, r );
			yLoc = get_y_loc( c, r );

			for( c = c0; c < maxCol; c += 2, ++xLoc, --yLoc )
			{
				if( xLoc >= 0 && xLoc < max_x_loc && yLoc >= 0 && yLoc < max_y_loc )
				{
					Location *locPtr = get_loc(xLoc,yLoc);
					x = image_x1 + calc_loc_pos_x(xLoc, yLoc);
					y = image_y1 + calc_loc_pos_y(xLoc, yLoc);
					draw_loc( x, y, xLoc, yLoc, locPtr );
				}
			}
		}
	}

	//---- derived function for ZoomMatrix & MapMatrix ----//

	post_draw();	// if they have anything to draw after the basic terrain (e.g.firms)

	//---------------------------------------------------//

	if( save_image_buf )
	{
		vga_back.read_bitmapW( image_x1, image_y1, image_x2, image_y2, save_image_buf );
		just_drawn_flag = 1;
	}
}
//------------ End of function Matrix::draw ------------//


//----------- Begin of function Matrix::disp ------------//
//
// Display the drawn world zoom on screen
//
void Matrix::disp()
{
	if( !just_drawn_flag )		// if the map has just been drawn in draw()
	{
		if( save_image_buf )
		{
			err_when( image_width%4 != 0 );
			vga_back.put_bitmapW( image_x1, image_y1, save_image_buf );
		}
		else
			draw();
	}

	just_drawn_flag=0;
}
//----------- End of function Matrix::disp ------------//


//--------- Begin of function Matrix::refresh ------------//
//
void Matrix::refresh()
{
	if( save_image_buf )	// redraw the background, so disp() won't use the old background
		draw();

	disp();
}
//----------- End of function Matrix::refresh ------------//


//----------- Begin of function Matrix::valid_cur_box ------------//
//
// Validate the position of the zoom area box, scroll window if necessary
// called by detect_area()
//
// Call valid_cur_box() if current highlight box is specified,
//                      adjust window area to fit it
//
// Call valid_disp_area() if window display area is specified,
//                        adjust highlight box to fit it
//
// [int] = callRefresh = call refresh() if scrolled
//                       (default : 1)
//
// return : <int> whether the window is scrolled
//
int Matrix::valid_cur_box(int callRefresh)
{
	int scrolledFlag=0;

	// calculate the boundary of top_x_loc and top_y_loc

	int topLocX1, topLocX2, topLocY1, topLocY2;
	if( !disp_rect_type )
	{
		topLocX1 = 0;
		topLocX2 = max_x_loc - disp_x_loc;
		topLocY1 = 0;
		topLocY2 = max_y_loc - disp_y_loc;
	}
	else
	{
		// such that middle bottom of display rectangle within (0, 0, max_x_loc-1, max_y_loc-1)
		// find the vector from top left to middle bottom of the display area in x,y location plane
		int middleBottomX = get_x_loc( disp_x_loc/2, disp_y_loc );
		int middleBottomY = get_y_loc( disp_x_loc/2, disp_y_loc );

		topLocX1 = - middleBottomX;
		topLocX2 = max_x_loc - middleBottomX;
		topLocY1 = - middleBottomY;
		topLocY2 = max_y_loc - middleBottomY;
	}

	//------- valid current highlight box first --------//
	if( !cur_cargo_rect_type )
	{
		// cur_cargo is a rectangle align with x, y location
		if( cur_x_loc+cur_cargo_width/2 < -cur_cargo_width )
			cur_x_loc = -cur_cargo_width;

		if( cur_y_loc < -cur_cargo_height )
			cur_y_loc = -cur_cargo_height;

		if( cur_x_loc+cur_cargo_width > max_x_loc )
			cur_x_loc = max_x_loc-cur_cargo_width;

		if( cur_y_loc+cur_cargo_height > max_y_loc )
			cur_y_loc = max_y_loc-cur_cargo_height;
	}
	else
	{
		// cur_cargo is a rectangle align with row column
		// middle buttom of the cargo is inside (0, 0, max_x_loc-1, max_y_loc-1)
		int middleBottomX = get_x_loc( cur_cargo_width / 2, cur_cargo_height );
		int middleBottomY = get_y_loc( cur_cargo_width / 2, cur_cargo_height );

		if( cur_x_loc + middleBottomX < 0 )
			cur_x_loc = -middleBottomX;

		if( cur_y_loc + middleBottomY < 0 )
			cur_y_loc = -middleBottomY;

		if( cur_x_loc + middleBottomX > max_x_loc )
			cur_x_loc = max_x_loc - middleBottomX;

		// ##### patch begin Gilbert 22/5 ######//
		if( cur_y_loc + middleBottomY > max_y_loc )
			cur_y_loc = max_y_loc - middleBottomY;
		// ##### patch end Gilbert 22/5 ######//
	}
	
	//--- scroll the display area to fit the current highlight ---//

	switch( (disp_rect_type << 8) + cur_cargo_rect_type )
	{
	case 0x0000:	// disp_rect_type == 0 && cur_cargo_rect_type == 0
		if( cur_x_loc < top_x_loc || cur_x_loc >= top_x_loc + disp_x_loc )
		{
			top_x_loc = cur_x_loc - (disp_x_loc - cur_cargo_width)/2;
			if( top_x_loc > topLocX2 )
				top_x_loc = topLocX2;
			if( top_x_loc < topLocX1 )
				top_x_loc = topLocX1;
			scrolledFlag=1;
		}

		if( cur_y_loc < top_y_loc || cur_y_loc >= top_y_loc + disp_y_loc )
		{
			top_y_loc = cur_y_loc - (disp_y_loc - cur_cargo_height)/2;
			if( top_y_loc > topLocY2 )
				top_y_loc = topLocY2;
			if( top_y_loc < topLocY1 )
				top_y_loc = topLocY1;
			scrolledFlag=1;
		}

		break;
	case 0x0001:	// disp_rect_type == 0 && cur_cargo_rect_type == 1
		err_here();
		break;
	case 0x0100:	// disp_rect_type == 1 && cur_cargo_rect_type == 0
		err_here();
		break;
	case 0x0101:	// disp_rect_type == 1 && cur_cargo_rect_type == 1
		{
			int curCol = get_column(cur_x_loc, cur_y_loc);
			int curRow = get_row(cur_x_loc, cur_y_loc);
			int topCol = get_column(top_x_loc, top_y_loc);
			int topRow = get_row(top_x_loc, top_y_loc);

			if( curCol + cur_cargo_width <= topCol || curCol >= topCol + disp_x_loc 
				|| curRow + cur_cargo_height <= topRow || curRow >= topRow + disp_y_loc )
			{
				if( curCol + cur_cargo_width <= topCol || curCol >= topCol + disp_x_loc )
					topCol = curCol - (disp_x_loc - cur_cargo_width)/2;
				if( curRow + cur_cargo_height <= topRow || curRow >= topRow + disp_y_loc )
					topRow = curRow - (disp_y_loc - cur_cargo_height)/2;

				// check top_x_loc, top_y_loc may be out of boundary
				top_x_loc = get_x_loc( topCol, topRow );
				top_y_loc = get_y_loc( topCol, topRow );
				if( top_x_loc > topLocX2 )
					top_x_loc = topLocX2;
				if( top_x_loc < topLocX1 )
					top_x_loc = topLocX1;
				if( top_y_loc > topLocY2 )
					top_y_loc = topLocY2;
				if( top_y_loc < topLocY1 )
					top_y_loc = topLocY1;
				scrolledFlag=1;
			}
		}
		break;
	default:
		err_here();
	}

	return scrolledFlag;
}
//----------- End of function Matrix::valid_cur_box ------------//


//--------- Begin of function Matrix::valid_disp_area ------//
//
// Validate the current location, adjust it if it is out of border
//
// Call valid_cur_box() if current highlight box is specified,
//                  adjust window area to fit it
//
// Call valid_disp_area() if window display area is specified,
//                      adjust highlight box to fit it
//
// [int] fitInBox = whether must fit in current box into the
//                  display area (default : 0)
//
void Matrix::valid_disp_area(int fitInBox)
{
	//------- valid display area first ---------//

	// calculate the boundary of top_x_loc and top_y_loc

	int topLocX1, topLocX2, topLocY1, topLocY2;
	if( !disp_rect_type )
	{
		topLocX1 = 0;
		topLocX2 = max_x_loc - disp_x_loc;
		topLocY1 = 0;
		topLocY2 = max_y_loc - disp_y_loc;
	}
	else
	{
		// such that middle bottom of display rectangle within (0, 0, max_x_loc-1, max_y_loc-1)
		// find the vector from top left to middle bottom of the display area in x,y location plane
		int middleBottomX = get_x_loc( disp_x_loc/2, disp_y_loc );
		int middleBottomY = get_y_loc( disp_x_loc/2, disp_y_loc );

		topLocX1 = - middleBottomX;
		topLocX2 = max_x_loc - middleBottomX;
		topLocY1 = - middleBottomY;
		topLocY2 = max_y_loc - middleBottomY;
	}

	if( top_x_loc > topLocX2 )
		top_x_loc = topLocX2;
	if( top_x_loc < topLocX1 )
		top_x_loc = topLocX1;
	if( top_y_loc > topLocY2 )
		top_y_loc = topLocY2;
	if( top_y_loc < topLocY1 )
		top_y_loc = topLocY1;


	//--- if the current highlighted location is outside the display area, then reposition it ----//

	if( fitInBox )
	{
		switch( (disp_rect_type << 8) + cur_cargo_rect_type )
		{
		case 0x0000:	// disp_rect_type == 0 && cur_cargo_rect_type == 0
			if( cur_x_loc < top_x_loc )
				cur_x_loc = top_x_loc;
			if( cur_x_loc + cur_cargo_width > top_x_loc + disp_x_loc )
				cur_x_loc = top_x_loc + disp_x_loc - cur_cargo_width;
			if( cur_y_loc < top_y_loc )
				cur_y_loc = top_y_loc;
			if( cur_y_loc + cur_cargo_height >= top_y_loc + disp_y_loc )
				cur_y_loc = top_y_loc + disp_y_loc - cur_cargo_height;
			break;
		case 0x0001:	// disp_rect_type == 0 && cur_cargo_rect_type == 1
			err_here();
			break;
		case 0x0100:	// disp_rect_type == 1 && cur_cargo_rect_type == 0
			err_here();
			break;
		case 0x0101:	// disp_rect_type == 1 && cur_cargo_rect_type == 1
			{
				int curCol = get_column(cur_x_loc, cur_y_loc);
				int curRow = get_row(cur_x_loc, cur_y_loc);
				int topCol = get_column(top_x_loc, top_y_loc);
				int topRow = get_row(top_x_loc, top_y_loc);

				if( curCol < topCol )
					curCol = topCol;
				if( curCol + cur_cargo_width >= topCol + disp_x_loc )
					curCol = topCol + disp_x_loc - cur_cargo_width;
				if( curRow < topRow )
					curRow = topRow;
				if( curRow + cur_cargo_height >= topRow + disp_y_loc )
					curRow = topRow + disp_y_loc - cur_cargo_height;

				cur_x_loc = get_x_loc( curCol, curRow );
				cur_y_loc = get_y_loc( curCol, curRow );
			}
			break;
		default:
			err_here();
		}

	}
}
//--------- End of function Matrix::valid_disp_area ------//


//---------- Begin of function Matrix::scroll -----------//
//
// <int> xScroll - horizontal scroll step (negative:left, positive:right)
// <int> yScroll - vertical scroll step   (negative:left, positive:right)
//
void Matrix::scroll(int xScroll, int yScroll)
{
	top_x_loc += xScroll;
	top_y_loc += yScroll;

	valid_disp_area();
}
//----------- End of function Matrix::scroll ------------//

//----------- Begin of function Matrix::get_detect_location ----------//
//
// find the location of the given screen coordinate
// return 1 if success
//
int Matrix::get_detect_location(int scrnX, int scrnY, int *locX, int *locY, char *mobileType)
{
	if( scrnX >= image_x1 && scrnX <= image_x2 && scrnY >= image_y1 && scrnY <= image_y2 )
	{
		int relX = scrnX - image_x1;
		int relY = scrnY - image_y1;
		int dummyX, dummyY;

		// try not to use divide becuase result division on a negative number is not desired
 		if( LOC_XY_MUL_SHIFT >= determinant_shift )
		{
			dummyX = top_x_loc + ((relX * loc_y_height - relY * loc_y_width ) << (LOC_XY_MUL_SHIFT - determinant_shift)); 
			dummyY = top_y_loc + ((relY * loc_x_width - relX * loc_x_height ) << (LOC_XY_MUL_SHIFT - determinant_shift)); 
		}
		else
		{
			dummyX = top_x_loc + ((relX * loc_y_height - relY * loc_y_width ) >> (determinant_shift - LOC_XY_MUL_SHIFT)); 
			dummyY = top_y_loc + ((relY * loc_x_width - relX * loc_x_height ) >> (determinant_shift - LOC_XY_MUL_SHIFT)); 
		}

		if( dummyX >= 0 && dummyX < max_x_loc && dummyY >= 0 && dummyY < max_y_loc )
		{
			*locX = dummyX;
			*locY = dummyY;
			return 1;
		}
	}

	return 0;
}
//----------- End of function Matrix::get_detect_location ----------//


//----------- Begin of function Matrix::get_close_location ----------//
//
// find the location of the given screen coordinate
// return 1 if success
//
int Matrix::get_close_location(int scrnX, int scrnY, int *locX, int *locY, char *mobileType)
{
	return 0;
}
//----------- End of function Matrix::get_close_location ----------//


//----------- Begin of function Matrix::get_column -----------//

int Matrix::get_column(int xLoc, int yLoc)
{
#if(RHOMBUS_LOCATION==1)
	return xLoc - yLoc;
#else
	return xLoc;
#endif
}

//----------- End of function Matrix::get_column -----------//


//----------- Begin of function Matrix::get_row -----------//

int Matrix::get_row(int xLoc, int yLoc)
{
#if(RHOMBUS_LOCATION==1)
	return xLoc + yLoc;
#else
	return yLoc;
#endif
}

//----------- End of function Matrix::get_row -----------//


//----------- Begin of function Matrix::get_x_loc -----------//

int Matrix::get_x_loc(int column, int row)
{
#if(RHOMBUS_LOCATION==1)
	return (column + row) >> 1;
	// use >> 1 instead of / 2 because -1 >> 1 return -1, but -1/2 returns 0
#else
	return column;
#endif
}

//----------- End of function Matrix::get_x_loc -----------//


//----------- Begin of function Matrix::get_y_loc -----------//

int Matrix::get_y_loc(int column, int row)
{
#if(RHOMBUS_LOCATION==1)
	return (row - column) >> 1;
	// use >> 1 instead of / 2 because -1 >> 1 return -1, but -1/2 returns 0
#else
	return row;
#endif
}
//----------- End of function Matrix::get_y_loc -----------//



// loc x/y to window position (relative to image_x1, image_y1)
int Matrix::calc_loc_pos_x(int xLoc, int yLoc)
{ 
	return ((xLoc-top_x_loc) * loc_x_width + (yLoc-top_y_loc) * loc_y_width) >> LOC_XY_MUL_SHIFT;
}


int Matrix::calc_loc_pos_y(int xLoc, int yLoc)
{
	return ((xLoc-top_x_loc) * loc_x_height + (yLoc-top_y_loc) * loc_y_height) >> LOC_XY_MUL_SHIFT; 
}

