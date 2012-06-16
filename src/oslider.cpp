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

//Filename    : OSLIDER.CPP
//Description : Object slider

#include <all.h>
#include <ovga.h>
#include <vga_util.h>
#include <osys.h>
#include <ofont.h>
#include <omouse.h>
#include <oslider.h>

//--------- Define macro constant -----------//

enum { SLIDER_HEIGHT = 16 };     // Default height of slider

//-------- Begin of function Slider::init --------//
//
// <int>  x1, y1       = location of the slider
// <int>  barWidth     = width of the slider bar
// [int]  sliderHeight = if 0, use the default SLIDER_HEIGHT
// <int>  barColor     = color of the slider bar
// <int*> varPtr       = pointer to the slider varialbe
// <int>  maxValue     = maximum value of the variable
//
// [int]  stdValue     = the standard value, color is different when it is
//		         below or above the stdValue
//		         (default : no standard value)
//
// [int]  barColor2    = the color when the current bar value > standard value
//		         only necessary when stdValue is passed
//
void Slider::init( int x1, int y1, int barWidth, int sliderHeight, int barColor,
						 int* varPtr, int maxValue, int stdValue, int barColor2 )
{
   slider_x1    = x1;
   slider_y1    = y1;
   slider_x2    = x1+barWidth+3;

   if( sliderHeight )
      slider_y2 = y1+sliderHeight-1;
   else
      slider_y2 = y1+SLIDER_HEIGHT-1;

   bar_width    = barWidth;
   bar_color    = barColor;

   var_ptr      = varPtr;
   max_value    = maxValue;

   std_value    = stdValue;
   bar_color2   = barColor2;

   paint();
}
//--------- End of function Slider::init ---------//


//-------- Begin of function Slider::paint --------//
//
void Slider::paint()
{
	vga_util.d3_panel_down( slider_x1, slider_y1, slider_x2, slider_y2 );

	refresh();
}
//--------- End of function Slider::paint ---------//


//-------- Begin of function Slider::refresh --------//
//
void Slider::refresh()
{
	err_when( max_value==0 );

	int barWidth = (*var_ptr) * bar_width / max_value;

	if( barWidth > bar_width )
		barWidth = bar_width;

	mouse.hide_area(slider_x1, slider_y1, slider_x2, slider_y2);

	//----- paint the bar area ---------//

	if( barWidth > 0 )
	{
		if( std_value )   // if standard value is set, than use different color
		{
			int stdBarWidth = std_value * bar_width / max_value;

			//--- use bar_color for the area < std_value ---//

			err_when( slider_x1+2 > slider_x1+1+MIN(barWidth,stdBarWidth) );

			Vga::active_buf->bar( slider_x1+2, slider_y1+2, slider_x1+1+MIN(barWidth,stdBarWidth),
				 slider_y2-2, bar_color );

			//--- use bar_color2 for the area > std_value ---//

			if( barWidth > stdBarWidth )
			{
				err_when( slider_x1+2+stdBarWidth > slider_x1+1+barWidth );

				Vga::active_buf->bar( slider_x1+2+stdBarWidth, slider_y1+2, slider_x1+1+barWidth,
					 slider_y2-2, bar_color2 );
			}
		}
		else
		{
			err_when( slider_x1+2 > slider_x1+1+barWidth );

			Vga::active_buf->bar( slider_x1+2, slider_y1+2, slider_x1+1+barWidth,
				slider_y2-2, bar_color );
		}
	}

	//----- paint the non-bar area ---------//

	if( barWidth < bar_width )
	{
      err_when( slider_x1+2+barWidth > slider_x2-2 );

		Vga::active_buf->bar( slider_x1+2+barWidth, slider_y1+2, slider_x2-2, slider_y2-2, Vga::active_buf->color_down );
	}

	// ##### begin Gilbert 14/10 ######//
	if( !vga.use_back_buf )
		vga_back.bar( slider_x1, slider_y1, slider_x2, slider_y2, V_RED );
	// ##### end Gilbert 14/10 ######//

	mouse.show_area();
}
//--------- End of function Slider::refresh ---------//


//-------- Begin of function Slider::detect --------//
//
// Return : 1-pressed on slider, variable updated
//	    0-not pressed on slider
//
int Slider::detect()
{
   if( !mouse.single_click( slider_x1, slider_y1, slider_x2, slider_y2 ) )
		return 0;

	int lastVar = *var_ptr;

	while( mouse.left_press )
	{
		sys.yield();
		mouse.get_event();

		*var_ptr = (long)(mouse.cur_x-slider_x1-2) * max_value / bar_width;

		if( *var_ptr < 0 )
			*var_ptr = 0;

		if( *var_ptr > max_value )
			*var_ptr = max_value;

		if( lastVar != *var_ptr )
			refresh();
	}

	return 1;
}
//--------- End of function Slider::detect ---------//


//.........................................................//


//-------- Begin of function SliderGroup::SliderGroup -------//

SliderGroup::SliderGroup(int sliderNum)
{
	slider_num     = sliderNum;
	slider_array   = new Slider[sliderNum];
}
//---------- End of function SliderGroup::SliderGroup -------//


//--------- Begin of function SliderGroup::paint ----------//
//
// Paint all sliders in this slider group.
//
void SliderGroup::paint()
{
   int i;

   for( i=0 ; i<slider_num ; i++ )
      slider_array[i].paint();
}
//----------- End of function SliderGroup::paint ----------//


//--------- Begin of function SliderGroup::detect ----------//
//
// Detect all sliders in this slider group.
// Since only one slider can be pressed at one time,
// so if any one of them is pressed, the previously pressed one
// will be pop up.
//
// Return : <int> 1 - slider detected
//                0 - not detected
//
int SliderGroup::detect()
{
	int i, oldValue;

	for( i=0 ; i<slider_num ; i++ )
	{
		oldValue = *(slider_array[i].var_ptr);

		if( slider_array[i].detect() )
			break;
   }

   if( i==slider_num )	// no detect
      return 0;


   //------- pull or give value to other slider ------//

   int j, decValue;
   int deltaValue = *(slider_array[i].var_ptr) - oldValue;


   //------- If it's the last slider ---------//

   if( i==slider_num-1 )
   {
      if( deltaValue < 0 ) // its value goes to the first slider
      {
			*(slider_array[0].var_ptr) += -deltaValue;
			slider_array[0].refresh();
		}
		else	// pull value from top sliders
		{
			for( j=0 ; j<slider_num-1 && deltaValue>0 ; j++ )
			{
				decValue = MIN( *(slider_array[j].var_ptr), deltaValue );

				*(slider_array[j].var_ptr) -= decValue;
				deltaValue		       -= decValue;

				slider_array[j].refresh();
			}
		}
	}

	//----- If it's not the last slider -----//

	else
	{
		if( deltaValue < 0 ) // its value goes to the last slider
		{
			*(slider_array[slider_num-1].var_ptr) += -deltaValue;
			slider_array[slider_num-1].refresh();
		}
		else	// pull value from bottom sliders
		{
			for( j=slider_num-1 ; j>=0 && deltaValue>0 ; j-- )
			{
				if( j != i )	// don't pull from itself
				{
					decValue = MIN( *(slider_array[j].var_ptr), deltaValue );

					*(slider_array[j].var_ptr) -= decValue;
					deltaValue		  -= decValue;

					slider_array[j].refresh();
				}
			}
		}
   }

   return 1;
}
//----------- End of function SliderGroup::detect ----------//

