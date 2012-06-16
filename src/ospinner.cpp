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

//Filename   : OSPINNER.CPP
//Description: class Spinner

#include <ospinner.h>
#include <oinfo.h>
#include <ovga.h>
#include <vga_util.h>
#include <ofont.h>
#include <oimgres.h>
#include <omouse.h>

//------ Define static class member vars ------//

Font* Spinner::default_font_ptr=NULL;
char* Spinner::default_spinner_icon=NULL;

//------- Begin of function Spinner::init -------//
//
// <int>   x1, y1		  - top-left coordination of the spinner description
// <char*> spinnerDes  - description of the spinner
//	<int>   x2 			  - x2 of the spinner description
// <int>   x3          - the ending x coordination of the spinner field
// <Font*> fontPtr	  - pointer of the font to be used with this spinner
//
void Spinner::init(int inX1, int inY1, char* spinnerDes, int inX2, int inX3, Font* fontPtr)
{
	font_ptr 	 = default_font_ptr;
	spinner_icon = default_spinner_icon;

	x1 = inX1;
	y1 = inY1;
	x2 = inX2;
	x3 = inX3;
	y2 = y1+font_ptr->height()+3;

	spinner_des = spinnerDes;

	option_count = 0;
	selected_id = 1;

	init_flag = 1;
}
//-------- End of function Spinner::init --------//


//------- Begin of function Spinner::set_default_icon -------//
//
void Spinner::set_default_icon(char* iconName)
{
	default_spinner_icon = image_icon.get_ptr(iconName);
}
//-------- End of function Spinner::set_default_icon --------//


//------- Begin of function Spinner::add_option -------//
//
// Add a spinner option.
//
// <char*> optionDes - pointer to the option description.
//
void Spinner::add_option(char* optionDes)
{
	err_when( !init_flag );

	err_when( option_count >= MAX_SPINNER_OPTION );

	option_des_array[option_count++] = optionDes;
}
//-------- End of function Spinner::add_option --------//


//------- Begin of function Spinner::disp -------//
//
// [int] refreshFlag = refresh flag
//							  (default: INFO_REPAINT)
//
void Spinner::disp(int refreshFlag)
{
	err_when( !init_flag );

	if( refreshFlag == INFO_REPAINT )
	{
		int y2 = y1+font_ptr->height()+3;

		font_ptr->put( x1, y1+2, spinner_des );

		vga_util.d3_panel_down( x2+3, y1, x3, y2 );

		vga_front.put_bitmap( x3+5, y1, spinner_icon );
	}

	font_ptr->disp( x2+6, y1+3, option_des_array[selected_id-1], x3-2 );
}
//-------- End of function Spinner::disp --------//


//------- Begin of function Spinner::detect -------//
//
int Spinner::detect()
{
	err_when( !init_flag );

	//----- detect clicking on the spinner icon -----//

	int iconWidth  = *((short*)spinner_icon);
	int iconHeight = *(((short*)spinner_icon)+1);

	if( !mouse.single_click( x3+3, y1, x3+2+iconWidth, y1+iconHeight-1 ) )
		return 0;

	//------ display the option list --------//

	return 0;
}
//-------- End of function Spinner::detect --------//

/*
//------- Begin of function Spinner::disp_list -------//
//
void Spinner::disp_list()
{
	err_when( !init_flag );

	int iconWidth  = *((short*)spinner_icon);
	int optionHeight = font_ptr->height()+3;

	int listX1 = x2;
	int listX2 = x3+iconWidth-1;

	int listY1 = y1 - (selected_id-1) * optionHeight;
	int listY2 = y2 - (option_count-1-(selected_id-1)) * optionHeight;


}
//-------- End of function Spinner::disp_list --------//
*/
