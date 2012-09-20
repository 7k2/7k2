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

//Filename    : OBUTT3D.CPP
//Description : 3D Button Object

#include <obutt3d.h>
#include <all.h>
#include <osys.h>
#include <ovga.h>
#include <vga_util.h>
#include <ohelp.h>
#include <omouse.h>
#include <oimgres.h>
#include <ocoltbl.h>
#include <otutor.h>
#include <ogame.h>

//---------- define static vars ----------//

static short save_back_buf[BUTTON_ACTION_WIDTH * BUTTON_ACTION_HEIGHT];


//-------- Begin of function Button3D::Button3D -------//
//
Button3D::Button3D()
{
	init_flag = 0;
	enable_flag = 0;
	button_key = 0;
	// ##### begin Gilbert 11/9 ######//
	visible_flag = 1;
	painted_flag = 0;
	// ##### end Gilbert 11/9 ######//
}
//--------- End of function Button3D::Button3D -------//


//-------- Begin of function Button3D::create -------//
//
// Create BUTTON_TYPE_SHELL button.
//
// create(<int>,<int>,<int>,<int>,<char>)
//
// <int>   x1, y1     = coordination of the button
// <char>  buttonStyle = style id. of the button
// <char*> buttonName = name of the button
// [int]   elasticFlag= Whether the button is elastic
//                      Elastic button will pop up immediately when button release
//                      Non-elastic button will remain pushed until pop() is called
//                      (default : 1)
// [int]   defIsPushed = default pushed_flag : 1-Pushed, 0-Non-pushed
//                       (default : 0)
//
void Button3D::create(int pX1, int pY1, char buttonStyle,
							 const char* buttonName, char elasticFlag, char defIsPushed)
{
	init_flag = 1;
	//------ set button parameters -----//

	button_type	 = BUTTON_TYPE_SHELL;
	button_style = buttonStyle;
	elastic_flag = elasticFlag;
	pushed_flag  = defIsPushed;
	enable_flag  = 1;
	// ##### begin Gilbert 11/9 ######//
	visible_flag = 1;
	painted_flag = 0;
	// ##### end Gilbert 11/9 ######//

	err_when( strlen(buttonName) > 8 );
	strcpy( help_code, buttonName );

	icon_ptr = image_button.get_ptr(buttonName);
	x1 = pX1;
	y1 = pY1;
	x2 = x1+ *((short*)icon_ptr) - 1;
	y2 = y1+ *((short*)(icon_ptr)+1) - 1;
}
//--------- End of function Button3D::create --------//


//-------- Begin of function Button3D::create -------//
//
// Create BUTTON_TYPE_BITMAP button.
//
void Button3D::create(int pX1, int pY1, const char* upButtonName,
							 const char* downButtonName, char elasticFlag, char defIsPushed)
{
	init_flag = 1;

	//--------- set button size --------//

	button_up_ptr   = image_button.get_ptr(upButtonName);
	icon_ptr = image_button.get_ptr(downButtonName);
	//	button_down_ptr = image_button.get_ptr(downButtonName);

	err_when( !button_up_ptr );
	err_when( !icon_ptr );

	x1 = pX1;
	y1 = pY1;
	x2 = x1+ *((short*)button_up_ptr) - 1;
	y2 = y1+ *((short*)(button_up_ptr)+1) - 1;

	//------ set button parameters -----//

	button_type	 = BUTTON_TYPE_BITMAP;
	elastic_flag = elasticFlag;
	pushed_flag  = defIsPushed;
	enable_flag  = 1;
	// ##### begin Gilbert 11/9 ######//
	visible_flag = 1;
	painted_flag = 0;
	// ##### end Gilbert 11/9 ######//

	err_when( strlen(upButtonName) > 8 );
	strcpy( help_code, upButtonName );
}
//--------- End of function Button3D::create --------//


//-------- Begin of function Button3D::update_bitmap -------//
//
// Update the bitmap of the button and repaint the button.
//
void Button3D::update_bitmap(const char* buttonName)
{
	err_when( strlen(buttonName) > 8 );
	strcpy( help_code, buttonName );

	icon_ptr = image_button.get_ptr(buttonName);

	paint();
}
//--------- End of function Button3D::update_bitmap --------//


//-------- Begin of function Button3D::set_help_code -------//
//
void Button3D::set_help_code(const char* helpCode)
{
	strncpy( help_code, helpCode, HELP_CODE_LEN );

	help_code[HELP_CODE_LEN] = '\0';
}
//--------- End of function Button3D::set_help_code --------//


//----------- Begin of function Button3D::paint -----------//
//
// [int] defIsPushed = default pushed_flag : 1-Pushed, 0-Non-pushed
//                       (default : pushed_flag)
//
void Button3D::paint(int defIsPushed)
{
	if( !init_flag )
		return;

	// #### begin Gilbert 11/9 ########//
	if( defIsPushed >= 0 )
		pushed_flag = defIsPushed;

	if( !visible_flag )
	{
		painted_flag = 0;
		return;
	}
	// #### end Gilbert 11/9 ########//

	if( !vga.use_back_buf )
		mouse.hide_area(x1, y1, x2, y2 );

	//------ display the button button -------//

	if( button_type == BUTTON_TYPE_SHELL && icon_ptr )
	{
		int tx = x1 + (BUTTON_ACTION_WIDTH - *((short*)icon_ptr))/2;
		int ty = y1 + (BUTTON_ACTION_HEIGHT - *((short*)icon_ptr+1))/2;
		//----------------------------------//

		if( enable_flag )
			Vga::active_buf->put_bitmap_trans_decompress( tx, ty, (char*) icon_ptr );   // 0 means not clear background
		else         // button disabled
		{
			// ##### begin Gilbert 24/12 #######//
			/*
			//--- put it on the back buffer, darken it and blt it back to the front buffer ---//

			if( !vga.use_back_buf )
				vga_back.read_bitmapW( x1, y1, x1+BUTTON_ACTION_WIDTH-1, y1+BUTTON_ACTION_HEIGHT-1, save_back_buf );

			//------ display and darken ----//

			vga_back.put_bitmap_trans_decompress( tx, ty, (char*) icon_ptr );   // 0 means not clear background

			vga_back.adjust_brightness( x1, y1, x1+BUTTON_ACTION_WIDTH-1, y1+BUTTON_ACTION_HEIGHT-1, -5 );

			//--- blt the button from the back to the front and restore the back buf ---//

			if( !vga.use_back_buf )
			{
				vga_util.blt_buf( x1, y1, x1+BUTTON_ACTION_WIDTH-1, y1+BUTTON_ACTION_HEIGHT-1, 0 );
				vga_back.put_bitmapW( x1, y1, save_back_buf );
			}
			*/

			// ##### begin Gilbert 2/1 #######//

			Vga::active_buf->put_bitmap_trans_remap_decompress( tx, ty, (char*) icon_ptr,
				(short *)vga.vga_color_table->get_table(-MAX_BRIGHTNESS_ADJUST_DEGREE*3/4) );

			// ##### end Gilbert 2/1 #######//
		}
	}
	else if( button_type == BUTTON_TYPE_BITMAP )
	{
		if( pushed_flag )
			Vga::active_buf->put_bitmap_trans_decompress( x1, y1, icon_ptr );
		else
			Vga::active_buf->put_bitmap_trans_decompress( x1, y1, button_up_ptr );
	}

	//--------------------------------------//

	if( !vga.use_back_buf )
		mouse.show_area();

	// #### begin Gilbert 11/9 ########//
	painted_flag = 1;
	// #### end Gilbert 11/9 ########//

#ifndef NO_REAL_TIME_UPDATE
	if( !vga.use_back_buf )
		sys.blt_virtual_buf_area( x1, y1, x2, y2 );
#endif
}
//---------- End of function Button3D::paint -----------//


//-------- Begin of function Button3D::detect -----------//
//
// Detect whether the button has been pressed,
// if so, act correspondly.
// Check for left mouse button only
//
// [unsigned] keyCode1 = if the specified key is pressed, emulate button pressed
//                       (default : 0)
//
// [unsigned] keyCode2 = if the specified key is pressed, emulate button pressed
//                       (default : 0)
//
// [int] detectRight   = whether also detect the right button or not
//                       (default : 0)
//
// [int] suspendPop    = don't pop up the button even it should
//                       (defalut : 0)
//
// Return : 1 - if left mouse button pressed
//          2 - if right mouse button pressed
//          3 - the key is pressed (only when keyCode is specified)
//          0 - if not
//
int Button3D::detect(unsigned keyCode1, unsigned keyCode2, int detectRight, int suspendPop)
{
	int rc=0;

	if( !init_flag )
		return 0;

	// ###### begin Gilbert 11/9 ######//
	if( !visible_flag || !painted_flag )
		return 0;
	// ###### end Gilbert 11/9 ######//

	help.set_help( x1, y1, x2, y2, help_code );
	
	if( !enable_flag )
		return 0;

	//---------------------------------------------//

	if( mouse.any_click(x1,y1,x2,y2,LEFT_BUTTON) )
		rc=1;

	else if( detectRight && mouse.any_click(x1,y1,x2,y2,RIGHT_BUTTON) )
		rc=2;

	else if(mouse.key_code)
	{
      unsigned mouseKey=mouse.key_code;

      if( mouseKey >= 'a' && mouseKey <= 'z' )   // non-case sensitive comparsion
         mouseKey -= 32;                         // convert from lower case to upper case

      if( mouseKey == keyCode1 || mouseKey == keyCode2 || mouseKey == button_key )
		{
         rc=3;
      }
   }

   if( !rc )
      return 0;

   //----- paint the button with pressed shape ------//

	#define PRESSED_TIMEOUT_SECONDS  1      // 1 seconds
	DWORD timeOutTime = misc.get_time()+PRESSED_TIMEOUT_SECONDS*1000;

	if( elastic_flag )
	{
		if( !pushed_flag )
			paint(1);

		while( (rc==1 && mouse.left_press) || (rc==2 && mouse.right_press) )
		{
			sys.yield();
			mouse.get_event();

			if( misc.get_time() >= timeOutTime )
				break;

			// leave the rectangle, cancel press
			if( mouse.cur_x < x1 || mouse.cur_x > x2 || mouse.cur_y < y1 || mouse.cur_y > y2 )
			{
				rc = 0;
				break;
			}
		}

		if( elastic_flag )
			paint(0);
	}
	else         // inelastic_flag button
	{
		if( suspendPop )
			pushed_flag = 1;
		else
			pushed_flag = !pushed_flag;

		paint(pushed_flag);

		while( (rc==1 && mouse.left_press) || (rc==2 && mouse.right_press) )
		{
			sys.yield();
			mouse.get_event();

			if( misc.get_time() >= timeOutTime )
				break;
		}
	}

	if (game.game_mode != GAME_TUTORIAL ||
		 (game.game_mode == GAME_TUTORIAL && tutor.allow_button_click(x1, y1, x2, y2, help_code, rc)))
		return rc;
	else
		return 0;
}
//----------- End of function Button3D::detect -------------//


//-------- Begin of function Button3D::hide -----------//
//
// Disable and hide the button.
//
void Button3D::hide()
{
	if( !init_flag )
		return;

	vga_util.blt_buf( x1, y1, x2, y2, 0 );

	// #### begin Gilbert 11/9 ######//
	// enable_flag = 0;
	visible_flag = 0;
	painted_flag = 0;
	// #### end Gilbert 11/9 ######//
}
//----------- End of function Button3D::hide -------------//


//-------- Begin of function Button3D::unhide -----------//
//
// unhide and show the button.
//
void Button3D::unhide()
{
	if( !init_flag )
		return;

	visible_flag = 1;
	painted_flag = 0;

	paint();
}
//----------- End of function Button3D::hide -------------//
