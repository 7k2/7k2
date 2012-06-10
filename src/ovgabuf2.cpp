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

//Filename    : OVGABUF2.CPP
//Description : OVGABUF direct draw surface class - part 2

#include <ddraw.h>
#include <all.h>
#include <imgfun.h>
#include <omouse.h>
#include <opower.h>
#include <ocoltbl.h>
#include <ovga.h>
#include <ovgabuf.h>
#include <omodeid.h>
#include <oimgres.h>
#include <ofont.h>
#include <obitmap.h>
#include <obitmapw.h>
#include <olinear.h>

//------- Define static class member vars ---------//

//char VgaBuf::color_light =(char)0x9E;    // color of the light panel side
//char VgaBuf::color_dark  =(char)0x99;    // color of the dark panel side
char VgaBuf::color_light =(char)126;    // color of the light panel side
char VgaBuf::color_dark  =(char)94;    // color of the dark panel side
//char VgaBuf::color_up	 =(char)0x9D;    // color of up_panel surface
//char VgaBuf::color_down  =(char)0x9C;    // color of down_panel surface
char VgaBuf::color_up	 =(char)126;    // color of up_panel surface
char VgaBuf::color_down  =(char)92;    // color of down_panel surface
char VgaBuf::color_push  =(char)0x9B;    // color of pushed button surface
char VgaBuf::color_border=(char)0x98;    // color of the border

// -------- define constant for d3_panel ------//

#define D3_PANEL_L1_X1Y1_R 150
#define D3_PANEL_L1_X1Y1_G 149
#define D3_PANEL_L1_X1Y1_B 70
#define D3_PANEL_L1_X2Y1_R 88
#define D3_PANEL_L1_X2Y1_G 84
#define D3_PANEL_L1_X2Y1_B 33
#define D3_PANEL_L1_X1Y2_R 77
#define D3_PANEL_L1_X1Y2_G 72
#define D3_PANEL_L1_X1Y2_B 38
#define D3_PANEL_L1_X2Y2_R 39
#define D3_PANEL_L1_X2Y2_G 40
#define D3_PANEL_L1_X2Y2_B 16

#define D3_PANEL_L2_X1Y1_R 248
#define D3_PANEL_L2_X1Y1_G 225
#define D3_PANEL_L2_X1Y1_B 105
#define D3_PANEL_L2_X2Y1_R 130
#define D3_PANEL_L2_X2Y1_G 118
#define D3_PANEL_L2_X2Y1_B 54
#define D3_PANEL_L2_X1Y2_R 109
#define D3_PANEL_L2_X1Y2_G 98
#define D3_PANEL_L2_X1Y2_B 44
#define D3_PANEL_L2_X2Y2_R 100
#define D3_PANEL_L2_X2Y2_G 97
#define D3_PANEL_L2_X2Y2_B 40

#define D3_PANEL_L3_COLOR 99
#define D3_PANEL_L4_COLOR 108


//------------- Begin of function VgaBuf::bar --------------//
//
// Draw a bar without bliting
//
// Syntax : bar( x1, y1, x2, y2, color )
//
// <int> x1,y1       - the top left vertex of the bar
// <int> x2,y2       - the bottom right vertex of the bar
// <int> color index - the index of a 256 color palette
//
void VgaBuf::bar(int x1,int y1,int x2,int y2,int colorCode)
{
	err_when( !buf_locked );

	if( is_front )
		mouse.hide_area(x1,y1,x2,y2);

	IMGbar(cur_buf_ptr, cur_pitch, x1, y1, x2, y2, translate_color(colorCode) );

	if( is_front )
		mouse.show_area();
}
//--------------- End of function VgaBuf::bar --------------//

/*
//------------- Begin of function VgaBuf::pixelize --------------//
//
// Put an area of evenly distributed pixels on the given area.
//
// <int> x1,y1     - the top left vertex of the bar
// <int> x2,y2     - the bottom right vertex of the bar
// <int> colorCode - colorCode of the pixels
//
void VgaBuf::pixelize(int x1,int y1,int x2,int y2,int colorCode)
{
	err_when( !buf_locked );

	if( is_front )
		mouse.hide_area(x1,y1,x2,y2);

   //---------------------------------------//

	// ###### begin Gilbert 7/7 #########//
	int	bufPitch = buf_pitch();
	int   x, y, lineRemainBytes = bufPitch - (x2-x1+1);
	char* writePtr = buf_ptr() + bufPitch * y1 + x1;

	for( y=y1 ; y<=y2 ; y++ )
	{
		if( y&1 )
		{
			writePtr+=bufPitch;
		}
		else
		{
			for( x=x1 ; x<=x2 ; x++, writePtr++ )
			{
				if( x&1 )
					*writePtr = colorCode;
			}

			writePtr+=lineRemainBytes;
		}
	}
	// ###### end Gilbert 7/7 #########//

	//---------------------------------------//

	if( is_front )
		mouse.show_area();
}
//--------------- End of function VgaBuf::pixelize --------------//
*/

//------------- Begin of function VgaBuf::rect --------------//
//
// Draw a rect on VGA screen
//
// Syntax : rect( x1, y1, x2, y2, lineWidth, color )
//
// int x1,y1       - the top left vertex of the rect
// int x2,y2       - the bottom right vertex of the rect
// int lineThick   - thickness of the lines of the rect
// int color       - the color of the rect
//
void VgaBuf::rect(int x1,int y1,int x2,int y2,int lt, int color)
{
	lt--;

	bar(x1,y1,x2,y1+lt,color);
	bar(x1,y1,x1+lt,y2,color);
	bar(x1,y2-lt,x2,y2,color);
	bar(x2-lt,y1,x2,y2,color);
}
//--------------- End of function VgaBuf::rect --------------//


//------------- Start of function VgaBuf::d3_rect --------------//
//
// Draw a d3 rect on VGA screen
//
// Syntax : d3_rect( x1, y1, x2, y2, lineWidth, color )
//
// int x1,y1       - the top left vertex of the d3_rect
// int x2,y2       - the bottom right vertex of the d3_rect

void VgaBuf::d3_rect(int x1,int y1,int x2,int y2)
{
	rect( x1+1, y1+1, x2, y2, 1, V_WHITE );
	rect( x1, y1, x2-1, y2-1, 1, VGA_GRAY+8 );
}
//--------------- End of function VgaBuf::d3_rect --------------//

/*

//------------- Start of function VgaBuf::tile --------------//
//
// Fill an area with a specific tile
//
// <int>   x1,y1    - the top left vertex of the tile
// <int>   x2,y2    - the bottom right vertex of the tile
// <char*> tileName - the name of the tile
//
void VgaBuf::tile(int x1,int y1,int x2,int y2,char* tileName)
{
   if( image_init_flag )
   {
      err_if( x1>x2 || y1>y2 || x1<0 || y1<0 || x2>=image_width || y2>=image_height )
         err_now( "VgaBuf::tile (image)" );

		char* tilePtr = icon_sym.get_ptr(tileName);

      if( tilePtr )
         IMGtile(x1,y1,x2,y2,tilePtr);
   }
   else
   {
      err_if( x1>x2 || y1>y2 || x1<0 || y1<0 || x2>=VGA_WIDTH || y2>=VGA_HEIGHT )
         err_now( "VgaBuf::tile (direct)" );

		if( is_front )
			mouse.hide_area( x1,y1,x2,y2 );

		char* tilePtr = icon_sym.get_ptr(tileName);

		if( tilePtr )
			VGAtile(x1,y1,x2,y2,tilePtr);

		if( is_front )
			mouse.show_area();
	}
}
//--------------- End of function VgaBuf::tile --------------//

*/

//------------- Start of function VgaBuf::separator --------------//
//
// Draw a VGA separator line
//
// Syntax : separator( x1, y1, x2, y2 )
//
// int x1,y1       - the top left vertex of the separator
// int x2,y2       - the bottom right vertex of the separator
//
void VgaBuf::separator(int x1, int y1, int x2, int y2)
{
   if( y1+1==y2 )       // horizontal line
   {
      bar( x1, y1, x2, y1, V_WHITE        );
      bar( x1, y2, x2, y2, color_dark );
	}
   else
   {
      bar( x1, y1, x1, y2, V_WHITE        );
      bar( x2, y1, x2, y2, color_dark );
   }
}
//--------------- End of function VgaBuf::separator --------------//


//------------- Start of function VgaBuf::indicator --------------//
//
// <int>   x1, y1, x2, y2 = coordination of the indicator
// <float> curValue       = the value of the bar
// <float> maxValue       = max value, the bar width = maxBarWidth * curValue / maxValue
// <int>   indiColor      = color of the indicator
// [int]   backColor      = background color
//                          (default : vga.color_down)
//			    (-2 if don't paint background color)
//
void VgaBuf::indicator(int x1, int y1, int x2, int y2, float curValue,
		    float maxValue, int indiColor, int backColor)
{
   if( backColor == -1 )
      backColor = color_down;

	/*
   if( curValue > maxValue )
		curValue = maxValue;

   if( curValue > 0 )
   {
      int barWidth = (int) ((float)(x2-x1) * curValue / maxValue);

		int halfHeight = (y2-y1+1)/2-1;
		int tx2        = x1+barWidth;
		int y;

		indiColor+=halfHeight;

		for( y=y1 ; y<y1+halfHeight ; y++, indiColor-- )
			bar( x1, y, tx2, y, indiColor );

		for( ; y<=y2 ; y++, indiColor++ )
			bar( x1, y, tx2, y, indiColor );

		if( backColor != -2 )	// -2 if don't paint background color
		{
			if( x1+barWidth < x2 )
			bar( x1+barWidth+1, y1, x2, y2, backColor );
		}
	}
	else
	{
		if( backColor != -2 )	// -2 if don't paint background color
			bar( x1, y1, x2, y2, backColor );
   }
	*/

	int cutPoint;
	if( curValue <= 0.0f )
		cutPoint = x1;
	else if( curValue >= maxValue )
		cutPoint = x2 + 1;
	else
		cutPoint = x1 + int(float(x2 - x1 + 1) * curValue / maxValue);

	if( cutPoint > x1 )
	{
		if( is_front )
			mouse.hide_area( x1, y1, cutPoint, y2 );

		int cutHeight = (y1 * 5 + y2 * 3) / 8;		// cut at 3/8 of between y1 and y2
		err_when( y2 - y1 - 1 < 4 );

		LinearCounter brightness;
		brightness.init( y1, 0, cutHeight, MAX_BRIGHTNESS_ADJUST_DEGREE-2 );		// glowing
		int y;
		for( y=y1; y <= cutHeight; y++, brightness.inc() )
		{
			barW_fast( x1, y, cutPoint, y, vga.vga_color_table->get_table(brightness.y)[indiColor]);
		}

		brightness.init( cutHeight, MAX_BRIGHTNESS_ADJUST_DEGREE-2, y2, -MAX_BRIGHTNESS_ADJUST_DEGREE/2 );		//
		for( ; y<=y2; ++y, brightness.inc() )
		{
			barW_fast( x1, y, cutPoint, y, vga.vga_color_table->get_table(brightness.y)[indiColor]);
		}

		if( is_front )
			mouse.show_area();
	}

	if( cutPoint <= x2 )
	{
		bar( cutPoint, y1, x2, y2, backColor );
	}
}
//--------------- End of function VgaBuf::indicator --------------//


//------------- Start of function VgaBuf::indicator --------------//
//
// <int>   barType        = bar style, bit 0 = disp curValue, bit 1 =  disp '/' and maxValue, bit 2 clear=mtr_b4 set=mtr_40, bit 3 = use another back buffer
// <int>   x1, y1         = coordination of the indicator
// <float> curValue       = the value of the bar
// <float> maxValue       = max value, the bar width = maxBarWidth * curValue / maxValue
// <int>   colorScheme    = color of the indicator // not used
//
void VgaBuf::indicator(int barType, int x1, int y1, float curValue, float maxValue, int colorScheme)
{
	int barWidth = 106;
	int barHeight = 35;
	int barLeftMargin = 15;
	int barRightMargin = 15;
	int barFluidMaxWidth = barWidth - barLeftMargin - barRightMargin;

	char *emptyBitmapName = barType & 2 ? (char*)"MTR_00" : (char*)"MTR_B1";
	char *fullBitmapName = barType & 2 ? (char*)"MTR_40" : (char*)"MTR_B4";

	int cutPoint;
	if( curValue <= 0.0f )
		cutPoint = barLeftMargin;
	else if( curValue >= maxValue )
		cutPoint = barWidth - barRightMargin;
	else
		cutPoint = barLeftMargin + int( (float)barFluidMaxWidth * curValue / maxValue );

	// ignore barType & 8;
	if( cutPoint > 0 )
		put_bitmap_area_trans( x1, y1, image_spict.read(fullBitmapName), 0, 0, cutPoint-1, barHeight-1 );
	if( cutPoint < barWidth )
		put_bitmap_area_trans( x1, y1, image_spict.read(emptyBitmapName), cutPoint, 0, barWidth-1, barHeight-1);

	if( barType & 1 )
	{
		String str;
		str = (int)curValue;
		if( barType & 2 )
		{
			str += "/";
			str += (int)maxValue;
		}
		font_hitpoint.center_put( x1, y1, x1+barWidth-1, y1+barHeight-1, str );
	}
}
//------------- End of function VgaBuf::indicator --------------//


//------------- Start of function VgaBuf::v_indicator --------------//
//
// Vertical indicator, all parameters are same as indicator() except
// it is vertical.
//
// <int>   x1, y1, x2, y2 = coordination of the indicator
// <float> curValue       = the value of the bar
// <float> maxValue       = max value, the bar width = maxBarWidth * curValue / maxValue
// <int>   indiColor      = color of the indicator
// [int]   backColor      = background color
//                          (default : vga.color_down)
//
void VgaBuf::v_indicator(int x1, int y1, int x2, int y2, float curValue,
                      float maxValue, int indiColor, int backColor)
{
   if( backColor == -1 )
      backColor = color_down;

	if( curValue > 0 )
   {
      int barHeight = (int) ((float)(y2-y1) * curValue / maxValue);

      int halfWidth = (x2-x1+1)/2-1;
		int ty1        = MAX(y2-barHeight,y1); // when curValue>0, even the actual bar width < 1, one pixel will also be painted
		int x;

		indiColor+=halfWidth;

		for( x=x1 ; x<x1+halfWidth ; x++, indiColor-- )
			bar( x, ty1, x, y2, indiColor );

		for( ; x<=x2 ; x++, indiColor++ )
			bar( x, ty1, x, y2, indiColor );

		if( y1 < y2-barHeight )
			bar( x1, y1, x2, y2-barHeight-1, backColor );
	}
	else
	{
		bar( x1, y1, x2, y2, backColor );
	}
}
//--------------- End of function VgaBuf::v_indicator --------------//


//---------- Begin of function VgaBuf::line -------------//
//
// Draw a line
//
// <int> x1,y1,x2,y2 = the coordination of the line
// <int> lineColor   = color of the line
//
void VgaBuf::line(int x1,int y1,int x2,int y2,int lineColor)
{
	if( is_front )
		mouse.hide_area( x1,y1,x2,y2 );  // if the mouse cursor is in that area, hide it

	IMGline(cur_buf_ptr, cur_pitch, buf_width(), buf_height(), x1, y1, x2, y2, translate_color(lineColor));

	if( is_front )
		mouse.show_area();
}
//------------ End of function VgaBuf::line -------------//


//---------- Begin of function VgaBuf::thick_line -------------//
//
// Draw a thick line
//
// <int> x1,y1,x2,y2 = the coordination of the line
// <int> lineColor   = color of the line
//
void VgaBuf::thick_line(int x1,int y1,int x2,int y2,int lineColor)
{
	err_when( x1<0 || y1<0 || x2>=VGA_WIDTH || y2>=VGA_HEIGHT );

	if( is_front )
		mouse.hide_area( x1,y1,x2,y2 );  // if the mouse cursor is in that area, hide it

	if( y1-y2 > abs(x2-x1) )   // keep thickness of the line to 3
	{
		IMGline(cur_buf_ptr, cur_pitch, buf_width(), buf_height(), x1  , y1-1, x2  , y2-1, lineColor );
		IMGline(cur_buf_ptr, cur_pitch, buf_width(), buf_height(), x1  , y1  , x2  , y2  , lineColor );
		IMGline(cur_buf_ptr, cur_pitch, buf_width(), buf_height(), x1  , y1+1, x2  , y2+1, lineColor );
		IMGline(cur_buf_ptr, cur_pitch, buf_width(), buf_height(), x1+1, y1+1, x2+1, y2+1, lineColor );
	}

	else if( y2-y1 > abs(x2-x1) )
	{
		IMGline(cur_buf_ptr, cur_pitch, buf_width(), buf_height(), x1+1, y1-1, x2+1, y2-1, lineColor );
		IMGline(cur_buf_ptr, cur_pitch, buf_width(), buf_height(), x1  , y1-1, x2  , y2-1, lineColor );
		IMGline(cur_buf_ptr, cur_pitch, buf_width(), buf_height(), x1  , y1  , x2  , y2  , lineColor );
		IMGline(cur_buf_ptr, cur_pitch, buf_width(), buf_height(), x1  , y1+1, x2  , y2+1, lineColor );
	}

	else
	{
		IMGline(cur_buf_ptr, cur_pitch, buf_width(), buf_height(), x1, y1-1, x2, y2-1, lineColor );
		IMGline(cur_buf_ptr, cur_pitch, buf_width(), buf_height(), x1, y1  , x2, y2  , lineColor );
		IMGline(cur_buf_ptr, cur_pitch, buf_width(), buf_height(), x1, y1+1, x2, y2+1, lineColor );
	}

	if( is_front )
      mouse.show_area();
}
//------------ End of function VgaBuf::thick_line -------------//


//----------- Begin of function VgaBuf::d3_panel_up ------------//
//
// Draw a Ms windows style 3D panel
//
// thickness can be 1-4
//
// <int> x1,y1,x2,y2 = the four vertex of the panel
// [int] thick       = thickness of the border
//                     (default:2)
// [int] paintCentre = paint the center area of the plane or not
//                     pass 0 if the area has just been painted with bar()
//                     (default:1)
//
void VgaBuf::d3_panel_up(int x1,int y1,int x2,int y2,int t,int paintCentre)
{
	// int i,x,y;

	err_when( x1>x2 || y1>y2 || x1<0 || y1<0 || x2>=VGA_WIDTH || y2>=VGA_HEIGHT );

	if( is_front )
		mouse.hide_area( x1,y1,x2,y2 );

	//------------------------------------------------//

	// ####### begin Gilbert 17/10 ##########//
	/*
	if( paintCentre )
		bar_fast(x1+2, y1+2, x2-3, y2-3, color_up );  // center

	//--------- white border on top and left sides -----------//

	bar_fast(x1,y1,x2-3,y1+1,0x9a );
	draw_pixel(x2-2, y1, 0x9a);
	bar_fast(x1,y1+2,x1+1,y2-3, 0x9a );    // left side
	draw_pixel(x1, y2-2, 0x9a);

	//--------- black border on bottom and right sides -----------//

	bar_fast(x2-2,y1+2,x2-1,y2-1, 0x90 );     // bottom side
	draw_pixel(x2-1, y1+1, 0x90);
	bar_fast(x1+2,y2-2,x2-3,y2-1, 0x90 );		 // right side
	draw_pixel(x1+1, y2-1, 0x90);

	//--------- junction between white and black border --------//
	draw_pixel(x2-1, y1, 0x97);
	draw_pixel(x2-2, y1+1, 0x97);
	draw_pixel(x1, y2-1, 0x97);
	draw_pixel(x1+1, y2-2, 0x97);

	//--------- gray shadow on bottom and right sides -----------//
	bar_fast(x2, y1+1, x2, y2, 0x97);
	bar_fast(x1+1, y2, x2-1, y2, 0x97);

	//-------------------------------------------//
	*/

	LinearCounter redCount, greenCount, blueCount;

	if( paintCentre )
	{
		bar_fast(x1+4, y1+4, x2-4, y2-4, color_up );  // center
	}

	short *pixelPtr;
	int pitch = buf_pitch();
	// outer layer, top border

	redCount.init(   x1, D3_PANEL_L1_X1Y1_R, x2, D3_PANEL_L1_X2Y1_R );
	greenCount.init( x1, D3_PANEL_L1_X1Y1_G, x2, D3_PANEL_L1_X2Y1_G );
	blueCount.init(  x1, D3_PANEL_L1_X1Y1_B, x2, D3_PANEL_L1_X2Y1_B );
	pixelPtr = buf_ptr( x1, y1 );
	for( ; !redCount.is_end(); redCount.inc(), greenCount.inc(), blueCount.inc(), ++pixelPtr )
	{
		*pixelPtr = vga.make_pixel(redCount.y, greenCount.y, blueCount.y);
	}

	// outer layer, bottom border
	redCount.init(   x1, D3_PANEL_L1_X1Y2_R, x2, D3_PANEL_L1_X2Y2_R );
	greenCount.init( x1, D3_PANEL_L1_X1Y2_G, x2, D3_PANEL_L1_X2Y2_G );
	blueCount.init(  x1, D3_PANEL_L1_X1Y2_B, x2, D3_PANEL_L1_X2Y2_B );
	pixelPtr = buf_ptr( x1, y2 );
	for( ; !redCount.is_end(); redCount.inc(), greenCount.inc(), blueCount.inc(), ++pixelPtr )
	{
		*pixelPtr = vga.make_pixel(redCount.y, greenCount.y, blueCount.y);
	}

	//outer layer left border
	redCount.init(   y1, D3_PANEL_L1_X1Y1_R, y2, D3_PANEL_L1_X1Y2_R );
	greenCount.init( y1, D3_PANEL_L1_X1Y1_G, y2, D3_PANEL_L1_X1Y2_G );
	blueCount.init(  y1, D3_PANEL_L1_X1Y1_B, y2, D3_PANEL_L1_X1Y2_B );
	pixelPtr = buf_ptr( x1, y1 );
	for( ; !redCount.is_end(); redCount.inc(), greenCount.inc(), blueCount.inc(), pixelPtr += pitch )
	{
		*pixelPtr = vga.make_pixel(redCount.y, greenCount.y, blueCount.y);
	}

	//outer layer right border
	redCount.init(   y1, D3_PANEL_L1_X2Y1_R, y2, D3_PANEL_L1_X2Y2_R );
	greenCount.init( y1, D3_PANEL_L1_X2Y1_G, y2, D3_PANEL_L1_X2Y2_G );
	blueCount.init(  y1, D3_PANEL_L1_X2Y1_B, y2, D3_PANEL_L1_X2Y2_B );
	pixelPtr = buf_ptr( x2, y1 );
	for( ; !redCount.is_end(); redCount.inc(), greenCount.inc(), blueCount.inc(), pixelPtr += pitch )
	{
		*pixelPtr = vga.make_pixel(redCount.y, greenCount.y, blueCount.y);
	}

	// inner layer, top border

	redCount.init(   x1+1, D3_PANEL_L2_X1Y1_R, x2-1, D3_PANEL_L2_X2Y1_R );
	greenCount.init( x1+1, D3_PANEL_L2_X1Y1_G, x2-1, D3_PANEL_L2_X2Y1_G );
	blueCount.init(  x1+1, D3_PANEL_L2_X1Y1_B, x2-1, D3_PANEL_L2_X2Y1_B );
	pixelPtr = buf_ptr( x1+1, y1+1 );
	for( ; !redCount.is_end(); redCount.inc(), greenCount.inc(), blueCount.inc(), ++pixelPtr )
	{
		*pixelPtr = vga.make_pixel(redCount.y, greenCount.y, blueCount.y);
	}

	// inner layer, bottom border
	redCount.init(   x1+1, D3_PANEL_L2_X1Y2_R, x2-1, D3_PANEL_L2_X2Y2_R );
	greenCount.init( x1+1, D3_PANEL_L2_X1Y2_G, x2-1, D3_PANEL_L2_X2Y2_G );
	blueCount.init(  x1+1, D3_PANEL_L2_X1Y2_B, x2-1, D3_PANEL_L2_X2Y2_B );
	pixelPtr = buf_ptr( x1+1, y2-1 );
	for( ; !redCount.is_end(); redCount.inc(), greenCount.inc(), blueCount.inc(), ++pixelPtr )
	{
		*pixelPtr = vga.make_pixel(redCount.y, greenCount.y, blueCount.y);
	}

	//inner layer left border
	redCount.init(   y1+1, D3_PANEL_L2_X1Y1_R, y2-1, D3_PANEL_L2_X1Y2_R );
	greenCount.init( y1+1, D3_PANEL_L2_X1Y1_G, y2-1, D3_PANEL_L2_X1Y2_G );
	blueCount.init(  y1+1, D3_PANEL_L2_X1Y1_B, y2-1, D3_PANEL_L2_X1Y2_B );
	pixelPtr = buf_ptr( x1+1, y1+1 );
	for( ; !redCount.is_end(); redCount.inc(), greenCount.inc(), blueCount.inc(), pixelPtr += pitch )
	{
		*pixelPtr = vga.make_pixel(redCount.y, greenCount.y, blueCount.y);
	}

	//inner layer right border
	redCount.init(   y1+1, D3_PANEL_L2_X2Y1_R, y2-1, D3_PANEL_L2_X2Y2_R );
	greenCount.init( y1+1, D3_PANEL_L2_X2Y1_G, y2-1, D3_PANEL_L2_X2Y2_G );
	blueCount.init(  y1+1, D3_PANEL_L2_X2Y1_B, y2-1, D3_PANEL_L2_X2Y2_B );
	pixelPtr = buf_ptr( x2-1, y1+1 );
	for( ; !redCount.is_end(); redCount.inc(), greenCount.inc(), blueCount.inc(), pixelPtr += pitch )
	{
		*pixelPtr = vga.make_pixel(redCount.y, greenCount.y, blueCount.y);
	}

	// layer 3
	if( x2-x1 >= 4 && y2-y1 >= 4 )
	{
		bar_fast( x1+2, y1+2, x2-2, y1+2, D3_PANEL_L3_COLOR );	// top
		bar_fast( x1+2, y2-2, x2-2, y2-2, D3_PANEL_L3_COLOR );	// bottom
		bar_fast( x1+2, y1+2, x1+2, y2-2, D3_PANEL_L3_COLOR );	// left
		bar_fast( x2-2, y1+2, x2-2, y2-2, D3_PANEL_L3_COLOR );	// right
	}

	// layer 4
	if( x2-x1 >= 6 && y2-y1 >= 6 )
	{
		bar_fast( x1+3, y1+3, x2-3, y1+3, D3_PANEL_L4_COLOR );	// top
		bar_fast( x1+3, y2-3, x2-3, y2-3, D3_PANEL_L4_COLOR );	// bottom
		bar_fast( x1+3, y1+3, x1+3, y2-3, D3_PANEL_L4_COLOR );	// left
		bar_fast( x2-3, y1+3, x2-3, y2-3, D3_PANEL_L4_COLOR );	// right
	}

	// ####### begin Gilbert 17/10 ##########//

	if( is_front )
		mouse.show_area();
}
//------------- End of function VgaBuf::d3_panel_up ------------//


//----------- Begin of function VgaBuf::d3_panel_down ------------//
//
// Draw a Ms windows style 3D panel with panel pushed downwards
//
// <int> x1,y1,x2,y2 = the four vertex of the panel
// [int] thick       = thickness of the border
//                     (default:2)
// [int] paintCentre = paint the center area of the plane or not
//                     pass 0 if the area has just been painted with bar()
//                     (default:1)
//
void VgaBuf::d3_panel_down(int x1,int y1,int x2,int y2,int t,int paintCentre)
{
	err_when( x1>x2 || y1>y2 || x1<0 || y1<0 || x2>=VGA_WIDTH || y2>=VGA_HEIGHT );

	if( is_front )
		mouse.hide_area( x1,y1,x2,y2 );

	//---------- main center area -----------//

	// ####### begin Gilbert 17/10 ##########//
	/*
	if( paintCentre )
		bar_fast(x1+2, y1+2, x2-3, y2-3, color_down );  // center

	//--------- black border on top and left sides -----------//

	bar_fast(x1,y1,x2-3,y1+1,0x90 );
	draw_pixel(x2-2, y1, 0x90);
	bar_fast(x1,y1+2,x1+1,y2-3, 0x90 );    // left side
	draw_pixel(x1, y2-2, 0x90);

	//--------- while border on bottom and right sides -----------//

	bar_fast(x2-2,y1+2,x2-1,y2-1, 0x9a );     // bottom side
	draw_pixel(x2-1, y1+1, 0x9a);
	bar_fast(x1+2,y2-2,x2-3,y2-1, 0x9a );		 // right side
	draw_pixel(x1+1, y2-1, 0x9a);

	//--------- junction between white and black border --------//
	draw_pixel(x2-1, y1, 0x97);
	draw_pixel(x2-2, y1+1, 0x97);
	draw_pixel(x1, y2-1, 0x97);
	draw_pixel(x1+1, y2-2, 0x97);

	//--------- remove shadow, copy from back  -----------//
	bar_fast(x2, y1+1, x2, y2, 0x9c);
	bar_fast(x1+1, y2, x2-1, y2, 0x9c);
*/

	LinearCounter redCount, greenCount, blueCount;

	if( paintCentre )
	{
		bar_fast(x1+5, y1+5, x2-3, y2-3, color_down );  // center
	}

	short *pixelPtr;
	int pitch = buf_pitch();
	// outer layer, top border

	redCount.init(   x1, D3_PANEL_L1_X1Y1_R, x2, D3_PANEL_L1_X2Y1_R );
	greenCount.init( x1, D3_PANEL_L1_X1Y1_G, x2, D3_PANEL_L1_X2Y1_G );
	blueCount.init(  x1, D3_PANEL_L1_X1Y1_B, x2, D3_PANEL_L1_X2Y1_B );
	pixelPtr = buf_ptr( x1, y1 );
	for( ; !redCount.is_end(); redCount.inc(), greenCount.inc(), blueCount.inc(), ++pixelPtr )
	{
		*pixelPtr = vga.make_pixel(redCount.y, greenCount.y, blueCount.y);
	}

	// outer layer, bottom border
	redCount.init(   x1, D3_PANEL_L1_X1Y2_R, x2, D3_PANEL_L1_X2Y2_R );
	greenCount.init( x1, D3_PANEL_L1_X1Y2_G, x2, D3_PANEL_L1_X2Y2_G );
	blueCount.init(  x1, D3_PANEL_L1_X1Y2_B, x2, D3_PANEL_L1_X2Y2_B );
	pixelPtr = buf_ptr( x1, y2 );
	for( ; !redCount.is_end(); redCount.inc(), greenCount.inc(), blueCount.inc(), ++pixelPtr )
	{
		*pixelPtr = vga.make_pixel(redCount.y, greenCount.y, blueCount.y);
	}

	//outer layer left border
	redCount.init(   y1, D3_PANEL_L1_X1Y1_R, y2, D3_PANEL_L1_X1Y2_R );
	greenCount.init( y1, D3_PANEL_L1_X1Y1_G, y2, D3_PANEL_L1_X1Y2_G );
	blueCount.init(  y1, D3_PANEL_L1_X1Y1_B, y2, D3_PANEL_L1_X1Y2_B );
	pixelPtr = buf_ptr( x1, y1 );
	for( ; !redCount.is_end(); redCount.inc(), greenCount.inc(), blueCount.inc(), pixelPtr += pitch )
	{
		*pixelPtr = vga.make_pixel(redCount.y, greenCount.y, blueCount.y);
	}

	//outer layer right border
	redCount.init(   y1, D3_PANEL_L1_X2Y1_R, y2, D3_PANEL_L1_X2Y2_R );
	greenCount.init( y1, D3_PANEL_L1_X2Y1_G, y2, D3_PANEL_L1_X2Y2_G );
	blueCount.init(  y1, D3_PANEL_L1_X2Y1_B, y2, D3_PANEL_L1_X2Y2_B );
	pixelPtr = buf_ptr( x2, y1 );
	for( ; !redCount.is_end(); redCount.inc(), greenCount.inc(), blueCount.inc(), pixelPtr += pitch )
	{
		*pixelPtr = vga.make_pixel(redCount.y, greenCount.y, blueCount.y);
	}

	// inner layer, top border

	redCount.init(   x1+1, D3_PANEL_L2_X1Y1_R, x2-1, D3_PANEL_L2_X2Y1_R );
	greenCount.init( x1+1, D3_PANEL_L2_X1Y1_G, x2-1, D3_PANEL_L2_X2Y1_G );
	blueCount.init(  x1+1, D3_PANEL_L2_X1Y1_B, x2-1, D3_PANEL_L2_X2Y1_B );
	pixelPtr = buf_ptr( x1+1, y1+1 );
	for( ; !redCount.is_end(); redCount.inc(), greenCount.inc(), blueCount.inc(), ++pixelPtr )
	{
		*pixelPtr = vga.make_pixel(redCount.y, greenCount.y, blueCount.y);
	}

	// inner layer, bottom border
	redCount.init(   x1+1, D3_PANEL_L2_X1Y2_R, x2-1, D3_PANEL_L2_X2Y2_R );
	greenCount.init( x1+1, D3_PANEL_L2_X1Y2_G, x2-1, D3_PANEL_L2_X2Y2_G );
	blueCount.init(  x1+1, D3_PANEL_L2_X1Y2_B, x2-1, D3_PANEL_L2_X2Y2_B );
	pixelPtr = buf_ptr( x1+1, y2-1 );
	for( ; !redCount.is_end(); redCount.inc(), greenCount.inc(), blueCount.inc(), ++pixelPtr )
	{
		*pixelPtr = vga.make_pixel(redCount.y, greenCount.y, blueCount.y);
	}

	//inner layer left border
	redCount.init(   y1+1, D3_PANEL_L2_X1Y1_R, y2-1, D3_PANEL_L2_X1Y2_R );
	greenCount.init( y1+1, D3_PANEL_L2_X1Y1_G, y2-1, D3_PANEL_L2_X1Y2_G );
	blueCount.init(  y1+1, D3_PANEL_L2_X1Y1_B, y2-1, D3_PANEL_L2_X1Y2_B );
	pixelPtr = buf_ptr( x1+1, y1+1 );
	for( ; !redCount.is_end(); redCount.inc(), greenCount.inc(), blueCount.inc(), pixelPtr += pitch )
	{
		*pixelPtr = vga.make_pixel(redCount.y, greenCount.y, blueCount.y);
	}

	//inner layer right border
	redCount.init(   y1+1, D3_PANEL_L2_X2Y1_R, y2-1, D3_PANEL_L2_X2Y2_R );
	greenCount.init( y1+1, D3_PANEL_L2_X2Y1_G, y2-1, D3_PANEL_L2_X2Y2_G );
	blueCount.init(  y1+1, D3_PANEL_L2_X2Y1_B, y2-1, D3_PANEL_L2_X2Y2_B );
	pixelPtr = buf_ptr( x2-1, y1+1 );
	for( ; !redCount.is_end(); redCount.inc(), greenCount.inc(), blueCount.inc(), pixelPtr += pitch )
	{
		*pixelPtr = vga.make_pixel(redCount.y, greenCount.y, blueCount.y);
	}

	// layer 3
	if( x2-x1 >= 4 && y2-y1 >= 4 )
	{
		bar_fast( x1+2, y1+2, x2-2, y1+3, D3_PANEL_L3_COLOR );	// top, width 2
		bar_fast( x1+2, y1+2, x1+3, y2-2, D3_PANEL_L3_COLOR );	// left, width 2
	}

	// layer 4
	if( x2-x1 >= 6 && y2-y1 >= 6 )
	{
		bar_fast( x1+4, y1+4, x2-2, y1+4, D3_PANEL_L4_COLOR );	// top
		bar_fast( x1+4, y2-2, x2-2, y2-2, D3_PANEL_L4_COLOR );	// bottom
		bar_fast( x1+4, y1+4, x1+4, y2-2, D3_PANEL_L4_COLOR );	// left
		bar_fast( x2-2, y1+4, x2-2, y2-2, D3_PANEL_L4_COLOR );	// right
	}

	// ####### end Gilbert 17/10 ##########//

	//----------- show mouse ----------//

	if( is_front )
		mouse.show_area();
}
//------------- End of function VgaBuf::d3_panel_down ------------//


//----------- Begin of function VgaBuf::d3_panel_up_clear ------------//
//
// clear the center of the either up or down panel
//
// <int> x1,y1,x2,y2 = the four vertex of the panel
// [int] thick       = thickness of the border
//                     (default:2)
//
void VgaBuf::d3_panel_up_clear(int x1,int y1,int x2,int y2,int t)
{
	// ####### begin Gilbert 19/10 #########//
	bar(x1+4, y1+4, x2-4, y2-4, color_up );
	// ####### end Gilbert 19/10 #########//
}
//------------- End of function VgaBuf::d3_panel_up_clear ------------//


//----------- Begin of function VgaBuf::d3_panel_down_clear ------------//
//
// clear the center of the either up or down panel
//
// <int> x1,y1,x2,y2 = the four vertex of the panel
// [int] thick       = thickness of the border
//                     (default:2)
//
void VgaBuf::d3_panel_down_clear(int x1,int y1,int x2,int y2,int t)
{
	// ####### begin Gilbert 19/10 #########//
	bar(x1+5, y1+5, x2-3, y2-3, color_down );
	// ####### end Gilbert 19/10 #########//
}
//------------- End of function VgaBuf::d3_panel_down_clear ------------//


//----------- Begin of function VgaBuf::adjust_brightness ------------//
//
// clear the center of the either up or down panel
//
// <int> x1,y1,x2,y2  = the four vertex of the panel
// <int> adjustDegree = the degree of brightness to adjust
//							   (a value from -10 to 10)
//
void VgaBuf::adjust_brightness(int x1,int y1,int x2,int y2,int adjustDegree)
{
	if( is_front )
		mouse.hide_area( x1,y1,x2,y2 );
#if( MAX_BRIGHTNESS_ADJUST_DEGREE > 10 )
	adjustDegree *= MAX_BRIGHTNESS_ADJUST_DEGREE / 10;
#endif

	err_when( adjustDegree < -MAX_BRIGHTNESS_ADJUST_DEGREE ||
				 adjustDegree >  MAX_BRIGHTNESS_ADJUST_DEGREE );

//	unsigned char* colorRemapTable = vga.vga_color_table->get_table(adjustDegree);
//	remap_bar(x1, y1, x2, y2, colorRemapTable);
	IMGbrightBar( cur_buf_ptr, cur_pitch, x1, y1, x2, y2, adjustDegree);

	if( is_front )
		mouse.show_area();
}
//------------- End of function VgaBuf::adjust_brightness ------------//


//----------- Begin of function VgaBuf::blt_buf_bright ----------//
//
// copy and change brightness of an area
//
void VgaBuf::blt_buf_bright( VgaBuf *srcBuf, int srcX1, int srcY1, int srcX2, int srcY2, int adjustDegree)
{
#if( MAX_BRIGHTNESS_ADJUST_DEGREE > 10 )
	adjustDegree *= MAX_BRIGHTNESS_ADJUST_DEGREE / 10;
#endif

	err_when( adjustDegree < -MAX_BRIGHTNESS_ADJUST_DEGREE ||
				 adjustDegree >  MAX_BRIGHTNESS_ADJUST_DEGREE );

	IMGcopyWbright( cur_buf_ptr, cur_pitch, srcBuf->cur_buf_ptr, srcBuf->cur_pitch, srcX1, srcY1, srcX2, srcY2, adjustDegree); 
}
//----------- End of function VgaBuf::blt_buf_bright ----------//


//----------- Begin of function VgaBuf::draw_d3_up_border ------------//
//
// Draw interface border.
//
void VgaBuf::draw_d3_up_border(int x1,int y1,int x2,int y2)
{
	// ##### begin Gilbert 19/10 #######//
/*
	err_when( x1>x2 || y1>y2 || x1<0 || y1<0 || x2>=VGA_WIDTH || y2>=VGA_HEIGHT );

	if( is_front )
		mouse.hide_area( x1,y1,x2,y2 );

	//--------- white border on top and left sides -----------//

	bar_fast( x1+1,y1,x2,y1, IF_LIGHT_BORDER_COLOR );    // top side
	bar_fast( x1,y1,x1,y2  , IF_LIGHT_BORDER_COLOR );    // left side

	//--------- black border on bottom and right sides -----------//

	bar_fast( x1+1,y2,x2,y2, IF_DARK_BORDER_COLOR );     // bottom side
	bar_fast( x2,y1+1,x2,y2, IF_DARK_BORDER_COLOR );		 // right side

	//-------------------------------------------//

	if( is_front )
		mouse.show_area();

*/
	d3_panel_up( x1, y1, x2, y2, 4, 0 );

	// ##### end Gilbert 19/10 #######//
}
//------------- End of function VgaBuf::draw_d3_up_border ------------//


//----------- Begin of function VgaBuf::draw_d3_down_border ------------//
//
// Draw interface border.
//
void VgaBuf::draw_d3_down_border(int x1,int y1,int x2,int y2)
{
	// #### begin Gilbert 19/10 #######//
/*
	err_when( x1>x2 || y1>y2 || x1<0 || y1<0 || x2>=VGA_WIDTH || y2>=VGA_HEIGHT );

	if( is_front )
		mouse.hide_area( x1,y1,x2,y2 );

	//--------- white border on top and left sides -----------//


	bar_fast( x1+1,y1,x2,y1, IF_DARK_BORDER_COLOR );    // top side
	bar_fast( x1,y1,x1,y2  , IF_DARK_BORDER_COLOR );    // left side

	//--------- black border on bottom and right sides -----------//

	bar_fast( x1+1,y2,x2,y2, IF_LIGHT_BORDER_COLOR );     // bottom side
	bar_fast( x2,y1+1,x2,y2, IF_LIGHT_BORDER_COLOR );		 // right side

	//-------------------------------------------//

	if( is_front )
		mouse.show_area();
*/

	d3_panel_down( x1, y1, x2, y2, 4, 0 );
	// ##### end Gilbert 19/10 ######//
}
//------------- End of function VgaBuf::draw_d3_down_border ------------//


//------------- Begin of function VgaBuf::blt_buf ------------//
// copy put whole part of a vgaBuf to (x1,y1) of this VgaBuf
void VgaBuf::blt_buf( VgaBuf *srcBuf, int x1, int y1 )
{
	short *srcPtr = srcBuf->cur_buf_ptr;
	int srcWidth = srcBuf->buf_width();
	int srcPitch = srcBuf->cur_pitch;
	int srcHeight = srcBuf->buf_height();
	short *destPtr = cur_buf_ptr;
	int destPitch = cur_pitch;

#ifdef ASM_FOR_MSVC
	_asm
	{
		mov	eax, y1
		imul	destPitch
		add	eax, x1
		add	eax, x1
		mov	edi, eax
		add	edi, destPtr

		mov	esi, srcPtr

		mov	ecx, srcHeight
blt_buf_1:
		push	ecx
		push	esi
		push	edi
		mov	ecx, srcWidth
		rep movsw
		pop	edi
		pop	esi
		pop	ecx
		add	edi, destPitch
		add	esi, srcPitch
		loop	blt_buf_1
	}
#endif
}
//------------- End of function VgaBuf::blt_buf ------------//


//-------- Begin of function VgaBuf::blt_virtual_buf --------//
//
// Blit entire source surface to local destination surface.
//
void VgaBuf::blt_virtual_buf( VgaBuf *srcBuf )
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
//--------- End of function VgaBuf::blt_virtual_buf ---------//


//-------- Begin of function VgaBuf::blt_virtual_buf_area --------//
//
// Blit an area of the source surface to local destination surface.
//
void VgaBuf::blt_virtual_buf_area( VgaBuf *srcBuf, int x1, int y1, int x2, int y2 )
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
//--------- End of function VgaBuf::blt_virtual_buf_area ---------//


//------------- Begin of function VgaBuf::bar_alpha --------------//
//
// Draw a bar with alpha-blendinig
//
// Syntax : bar( x1, y1, x2, y2, color )
//
// <int> x1,y1       - the top left vertex of the bar
// <int> x2,y2       - the bottom right vertex of the bar
// <int> logAlpha    - negative log alpha (right bit shift count, 0 to 5)
// <int> color index - the index of a 256 color palette
//
// logAlpha : 0=transparent ... 5=opaque colorCode
//
void VgaBuf::bar_alpha(int x1,int y1,int x2,int y2,int logAlpha, int colorCode)
{
	err_when( !buf_locked );

	if( logAlpha <= 0 )
	{
		err_when(logAlpha < 0);
		// if logAlpha == 0, no change
	}
	else if( logAlpha < 5 )
	{
		if( is_front )
			mouse.hide_area(x1,y1,x2,y2);
		IMGbarAlpha(cur_buf_ptr, cur_pitch, x1, y1, x2, y2, logAlpha, translate_color(colorCode) );
		if( is_front )
			mouse.show_area();
	}
	else	// logAlpha >= 5, equivalence to VgaBuf::bar
	{
		if( is_front )
			mouse.hide_area(x1,y1,x2,y2);
		IMGbar(cur_buf_ptr, cur_pitch, x1, y1, x2, y2, translate_color(colorCode) );
		if( is_front )
			mouse.show_area();
	}
}
//--------------- End of function VgaBuf::bar_alpha --------------//
