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

//Filename    : OSYSDISP.CPP
//Description : class Sys - display functions

#include <osys.h>
#include <ovga.h>
#include <vga_util.h>
#include <oinfo.h>
#include <obox.h>
#include <omodeid.h>
#include <oconfig.h>
#include <oingmenu.h>
#include <ooptmenu.h>
#include <obutt3d.h>
#include <ofirm.h>
#include <ocampgn.h>
#include <otown.h>
#include <olog.h>
#include <otornado.h>
#include <owarpt.h>
#include <osite.h>
#include <ofont.h>
#include <omusic.h>
#include <obutton.h>
#include <omouse.h>
#include <ohelp.h>
#include <owaypnt.h>
#include <oanline.h>
#include <onews.h>
#include <ogame.h>
#include <oworldmt.h>
#include <oremote.h>
#include <oaudio.h>
#include <onation.h>
#include <ovgalock.h>
#include <otutor.h>
#include <ot_gmenu.h>
#include <dbglog.h>

DBGLOG_DEFAULT_CHANNEL(Sys);

//---------- define static variables ----------//

static Button3D  button_menu;

//-------- Begin of function Sys::disp_button --------//
//
void Sys::disp_button()
{
	vga.use_back();
	if (current_display_mode.mode_id == MODE_ID_800x600x16)
		button_menu.paint( MENU_BUTTON_X1, MENU_BUTTON_Y1, "MENU-U", "MENU-D" );
	else
		button_menu.paint( MENU_BUTTON_X1, MENU_BUTTON_Y1, "MU_1024", "MD_1024" );
	button_menu.set_help_code( "GAMEMENU" );

	vga.use_front();
}
//--------- End of function Sys::disp_button ---------//


//-------- Begin of function Sys::detect_button --------//
//
void Sys::detect_button()
{
	//--------- detect menu button -------//

	if( button_menu.detect() )
	{
		in_game_menu.enter(!remote.is_enable());
		return;
	}

	//------- detect scroll menu buttons -----//

	#define VIEW_MODE_BUTTON_WIDTH   75 //58
	#define VIEW_MODE_BUTTON_HEIGHT  22 //16

	int VIEW_MODE_BUTTON_X_SPACE; 
	int i, x, y;
	if (current_display_mode.mode_id == MODE_ID_800x600x16)
	{
		VIEW_MODE_BUTTON_X_SPACE = VIEW_MODE_BUTTON_WIDTH + 2;
		x = 6;
		y = 7;
	}
	else if (current_display_mode.mode_id == MODE_ID_1024x768x16)
	{
		VIEW_MODE_BUTTON_X_SPACE = VIEW_MODE_BUTTON_WIDTH + 37;
		x = 819;
		y = 80;
	}
	else if (current_display_mode.mode_id == MODE_ID_1600x900x16)
	{
		VIEW_MODE_BUTTON_X_SPACE = VIEW_MODE_BUTTON_WIDTH + 37;
		x = 1395;
		y = 80;
	}
	else
		err_here();

	
	static char viewModeArray[] =
	{
		MODE_NATION, MODE_TOWN, MODE_ECONOMY, MODE_TRADE, MODE_MILITARY, MODE_TECH, MODE_SPY, MODE_RANK
	};
	for( i=0 ; i<8 ; i++, x+=VIEW_MODE_BUTTON_X_SPACE )
	{
		if(( ((i-1) % 2) == 1) && (current_display_mode.mode_id == MODE_ID_1600x900x16))
		{
			x = 1395;
			y = y + VIEW_MODE_BUTTON_HEIGHT;
		}

		if(( ((i-1) % 2) == 1) && (current_display_mode.mode_id == MODE_ID_1024x768x16))
		{
			x = 819;
			y = y + VIEW_MODE_BUTTON_HEIGHT;
		}

		if(( i==4 ) && (current_display_mode.mode_id == MODE_ID_800x600x16))		// the second row
		{
			x = 6;
			y = 29;
		}

		if( nation_array.player_recno==0 && i<7 )		// when the player has lost the game, the only report available is ranking report only
			continue;

		if( mouse.single_click( x, y, x+VIEW_MODE_BUTTON_WIDTH-1, y+VIEW_MODE_BUTTON_HEIGHT-1 ) )
		{
			int newMode = viewModeArray[i];

			if( view_mode == newMode )       	// when click on the same mode again, go to the normal mode
				set_view_mode(MODE_NORMAL);
			else
				set_view_mode(newMode);

			break;
		}
	}
}
//--------- End of function Sys::detect_button ---------//


//-------- Begin of function Sys::disp_frame --------//
//
// [int] dispCampaignMsg - whether display a campaign message in the beginning of the game or not 
//									(default: 0)
// 
void Sys::disp_frame(int dispCampaignMsg)
{
	if( sys.signal_exit_flag )
		return;

#ifdef DEBUG
	unsigned long startTime = misc.get_time();
	unsigned long updateViewTime = 0;
	unsigned long infoUpdateTime = 0;
	unsigned long dispMapTime = 0;
	unsigned long flipTime = 0;
#endif

	if( option_menu.is_active() )
	{
		// ##### begin Gilbert 3/11 ######//
		option_menu.disp(need_redraw_flag);
		// ##### end Gilbert 3/11 ######//
		blt_virtual_buf();
	}
	else
	{
		// -------- re-draw the whole screen if needed, such as after task switching ---------//

		if( need_redraw_flag )
		{
			// ###### begin Gilbert 4/1 ######//
			help.short_front_buf.clear();
			help.short_back_buf.clear();
			// ###### end Gilbert 4/1 ######//
			info.disp_panel();
			world.paint();
			disp_button();
			world.refresh();
			disp_view();
			disp_map();

			if( in_game_menu.is_active() )
			{
				vga.use_back();
				in_game_menu.disp();
				vga.use_front();
			}

			//------------ blt buffer --------------//

			vga_util.blt_buf(0,0, VGA_WIDTH-1, VGA_HEIGHT-1, 0);

			//------ if this is a campaign game -------//

			if( dispCampaignMsg )
			{
				//---- display campaign game intro ----//

				game.campaign()->disp_in_game_msg( game.campaign()->intro_text() );

				//------- display plot ---------//

				// ######## begin Gilbert 1/4 #########//
				if( game.campaign()->plot_id )
				{
					char *plotTextPtr = game.campaign()->plot_text();
					if( plotTextPtr )
					{
					//	vga.flip();
						game.campaign()->disp_in_game_msg( plotTextPtr );
					}
				}
				// ######## end Gilbert 1/4 #########//
				vga.flip();
			}

			//------------------------------------//

			disp_view_mode();

			info.disp();
		}
		else
		{
#ifdef DEBUG
			updateViewTime = misc.get_time();
#endif

			update_view();

#ifdef DEBUG
			updateViewTime = misc.get_time() - updateViewTime;
			infoUpdateTime = misc.get_time();
#endif 

			info.update();

#ifdef DEBUG
			infoUpdateTime = misc.get_time() - infoUpdateTime;
#endif 

		//-------- display the mini map ----------//

#ifdef DEBUG
			dispMapTime = misc.get_time();
#endif
			disp_map();
#ifdef DEBUG
			dispMapTime = misc.get_time() - dispMapTime;
#endif
	
			//------ display tutorial text -------//
			if( game.game_mode == GAME_TUTORIAL )
				tutor.disp();
		}

		//--------- display the map and info area --------//

		// ##### begin Gilbert 9/11 #####//
		help.disp_short_help(&vga_back);
		// both surface has its own save buffer (short_front_buf and short_back_buf)
		// disp_short_help saves to short_back_buf
		// ##### end Gilbert 9/11 #####//

		// skip flipping when in odd frame of report mode
		// report mode has not been drawn in back buffer
		if( !report_disp_frame_no )
		{
#if(defined(USE_FLIP) )
	#ifdef DEBUG
			flipTime = misc.get_time();
	#endif
			vga.flip();
	#ifdef DEBUG
			flipTime = misc.get_time() - flipTime;
	#endif
			help.flip();			// exchange short_front_buf and short_back_buf
#endif
		}

		// ##### begin Gilbert 9/11 #####//
		help.hide_short_help(&vga_back);
		// restore short_back_buf which was short_front_buf before flipping
		// ##### end Gilbert 9/11 #####//

		blt_virtual_buf();

		//---------- display help ----------//

		help.disp();
	}

	anim_line.inc_phase();		// originally in Sys::process()

	need_redraw_flag = 0;

#ifdef DEBUG
	startTime = misc.get_time() - startTime;
//	{
//		VgaFrontLock vgaLock;
		startTime = 0;			// set break point here
//	}
#endif
}
//-------- End of function Sys::disp_frame --------//


//-------- Begin of function Sys::disp_map --------//

void Sys::disp_map()
{
	//------ draw and display the map -------//

	if( map_need_redraw )		// requested by other modules to redraw the pre-drawn map background
	{
		world.map_matrix->draw();
		map_need_redraw = 0;
	}

	world.map_matrix->disp();

	//-------- draw dots on the map ---------//

	MapMatrix* mapMatrix = world.map_matrix;

	if( !mapMatrix->filter_object_flag || mapMatrix->filter_object_type == OBJ_FIRM )
		firm_array.draw_dot(mapMatrix->filter_object_para);

	if( !mapMatrix->filter_object_flag || mapMatrix->filter_object_type == OBJ_TOWN )
		town_array.draw_dot(mapMatrix->filter_object_para);

	if( !mapMatrix->filter_object_flag && !mapMatrix->filter_nation_flag )
		site_array.draw_dot();

	if( !mapMatrix->filter_object_flag || mapMatrix->filter_object_type == OBJ_UNIT )
	{
		unit_array.draw_dot(mapMatrix->filter_object_para);
		way_point_array.draw(1);		// draw on the mini-map
	}

	war_point_array.draw_dot();	// draw on the mini-map

	if( !mapMatrix->filter_object_flag && !mapMatrix->filter_nation_flag )
		tornado_array.draw_dot();

	mapMatrix->draw_square();		// draw a square on the map for current zoomed area

	//------- blt the map area to the front screen --------//

#if(!defined(USE_FLIP))
	vga_util.blt_buf( MAP_X1, MAP_Y1 , MAP_X2 , MAP_Y2);
#endif
}
//-------- End of function Sys::disp_map --------//


//-------- Begin of function Sys::disp_zoom --------//

void Sys::disp_zoom()
{
	//--------- set zoom window ----------//

	ZoomMatrix* zoomMatrix = world.zoom_matrix;

	//--------- draw map area ---------//

	if( zoom_need_redraw )		// requested by other modules to redraw the pre-drawn zoom background
	{
		long backupSeed = misc.get_random_seed();

		// #### begin Gilbert 8/2 #####//
		world.zoom_matrix->just_drawn_flag = 0;	// request world.zoom_matrix->disp() to call draw()
		// #### end Gilbert 8/2 #####//
		// world.zoom_matrix->draw();
		zoom_need_redraw = 0;
	}

	//-------- disp zoom area --------//

#ifdef DEBUG
	unsigned long dispTime = misc.get_time();
#endif
	world.zoom_matrix->disp();
#ifdef DEBUG
	dispTime = misc.get_time() - dispTime;
#endif

	//---- draw sprite white sites if in debug mode ----//

	#ifdef DEBUG
	if(debug2_enable_flag)
		world.zoom_matrix->draw_white_site();
	#endif

	//------- draw foreground objects --------//

#ifdef DEBUG
	unsigned long drawFrameTime = misc.get_time();
#endif
	world.zoom_matrix->draw_frame();
#ifdef DEBUG
	drawFrameTime = misc.get_time() - drawFrameTime;
#endif


#ifdef DEBUG
	unsigned long miscTime = misc.get_time();
#endif

	//----- draw the frame of the selected firm/town -----//

//	info.draw_selected();		// moved to world_z.cpp

	//-------- display news messages ---------//

	news_array.disp();

	//----- next frame, increase the frame counter -----//

	sys.frames_in_this_second++;		// no. of frames displayed in this second

	if( view_mode==MODE_NORMAL )
		disp_frames_per_second();

#ifdef DEBUG
	miscTime = misc.get_time() - miscTime;
	vga_front.temp_unlock();
	vga_back.temp_unlock();
	// set break point here
	vga_back.temp_restore_lock();
	vga_front.temp_restore_lock();
#endif
}
//-------- End of function Sys::disp_zoom --------//


//-------- Begin of function Sys::blt_virtual_buf --------//
//
void Sys::blt_virtual_buf()
{
//	if( !sys.debug_session )
	if( !use_true_front )
		return;

	//--- in a debug sesion, vga_front is not the true front buffer, now copy it to the true one ---//

#ifdef DEBUG
	unsigned long startTime = misc.get_time();
#endif
	int frontLocked=0;

	if( vga_front.buf_locked )
	{
		vga_front.unlock_buf();
		frontLocked=1;
	}

	vga_true_front.blt_virtual_buf( &vga_front );

	if( frontLocked )
		vga_front.lock_buf();

#ifdef DEBUG
	startTime = misc.get_time() - startTime;
	startTime = 0;
#endif
}
//--------- End of function Sys::blt_virtual_buf ---------//


//-------- Begin of function Sys::blt_virtual_buf_area --------//
//
void Sys::blt_virtual_buf_area(int x1, int y1, int x2, int y2)
{
//	if( !sys.debug_session )
	if( !use_true_front )
		return;

	if( x1 < 0 )
		x1 = 0;
	if( x2 >= VGA_WIDTH )
		x2 = VGA_WIDTH-1;
	if( y1 < 0 )
		y1 = 0;
	if( y2 >= VGA_HEIGHT )
		y2 = VGA_HEIGHT-1;

	//--- in a debug sesion, vga_front is not the true front buffer, now copy it to the true one ---//

	vga_front.temp_unlock();

	vga_true_front.blt_virtual_buf_area( &vga_front, x1, y1, x2, y2 );

	vga_front.temp_restore_lock();
}
//--------- End of function Sys::blt_virtual_area ---------//


//-------- Begin of function Sys::disp_frames_per_second --------//
//
void Sys::disp_frames_per_second()
{
	if( !config.show_debug_info )			// only display this in a debug session
		return;

	if( game.game_mode == GAME_TUTORIAL )		// don't display in tutorial mode as it overlaps with the tutorial text
		return;

	//------- get the curren system time ---------//

	DWORD curTime = misc.get_time();		// in millisecond

	//----------- first time calling -------------//

	if( last_second_time==0 )
	{
		last_second_time  = curTime;
		frames_in_this_second = 0;		// no. of frames displayed in this second
		return;
	}

	//------ when passes to the next second -----//

	if( curTime >= last_second_time+1000 )  // 1 second = 1000 millisecond
	{
		frames_per_second = frames_in_this_second;

		//------ update var and reset counter -----//

		last_second_time += 1000;
		frames_in_this_second = 0;
	}

	//---------- display frame count -----------//

	String str;

	str  = "Frames per second: ";
	str += frames_per_second;

	vga.use_back();

	font_news.disp( ZOOM_X1+10, ZOOM_Y1+10, str, MAP_X2);

	vga.use_front();
}
//--------- End of function Sys::disp_frames_per_second ---------//


//-------- Begin of function Sys::change_mode --------//

int Sys::change_display_mode(int modeId)
{
	if( modeId == current_display_mode.mode_id )
		return 0;
	
	UCHAR	tempVgaFrontLockStackCount = vga_front.lock_stack_count;
	WORD	tempVgaFrontLockBitStack =   vga_front.lock_bit_stack;
	UCHAR	tempVgaBackLockStackCount = vga_back.lock_stack_count;
	WORD	tempVgaBackLockBitStack =   vga_back.lock_bit_stack;
	BOOL    tempVgaFrontBufLocked = vga_front.buf_locked;
	BOOL    tempVgaBackBufLocked = vga_back.buf_locked;

	if( mouse.init_flag )
	{
		mouse.hide();
	}

	// unlock surface

	if( vga_back.is_inited() && vga_back.buf_locked )
	{
		DEBUG_LOG("Attempt vga_back.unlock_buf()");
		vga_back.unlock_buf();
		DEBUG_LOG("vga_back.unlock_buf() finish");
	}

	if( vga_front.is_inited() && vga_front.buf_locked )
	{
		DEBUG_LOG("Attempt vga_front.unlock_buf()");
		vga_front.unlock_buf();
		DEBUG_LOG("vga_front.unlock_buf() finish");
	}

	// deinit surface

	DEBUG_LOG("Attempt vga_back.deinit()");
	vga_back.deinit();
	DEBUG_LOG("vga_back.deinit() finish");

	DEBUG_LOG("Attempt vga_front.deinit()");
	vga_front.deinit();
	DEBUG_LOG("Attempt vga_front.deinit() finish");

	if( use_true_front )
	{
		DEBUG_LOG("Attempt vga_true_front.deinit()");
		vga_true_front.deinit();
		DEBUG_LOG("vga_true_front.deinit() finish");
	}

	// change display mode

	int oldMode = current_display_mode.mode_id;
	DisplayModeInfo *displayModeInfo = DisplayModeInfo::get_display_info(modeId);
	int rc = 0;

	if( displayModeInfo )
	{
		DEBUG_LOG("Attemp vga.change_resolution()");
		DisplayModeInfo::set_current_display_mode(modeId);
		if( vga.change_resolution(displayModeInfo->screen_width, displayModeInfo->screen_height) )
			rc = 1;
		DEBUG_LOG("Attemp vga.change_resolution() finish");
	}

	if( !rc )
	{
		// cannot change mode, remain at the same mode
		modeId = oldMode;
		displayModeInfo = DisplayModeInfo::get_display_info(modeId);
		if( !vga.change_resolution(displayModeInfo->screen_width, displayModeInfo->screen_height) )
		{
			sys.signal_exit_flag = 1;
			return 0;
		}
	}

	if( mouse.init_flag )
	{
		mouse.reset_boundary();		// since screen dimension changed
		mouse.show();
	}

	// update the boundary of anim_line
	anim_line.init(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2);

	vga_front.lock_stack_count = tempVgaFrontLockStackCount;
	vga_front.lock_bit_stack =   tempVgaFrontLockBitStack;
	vga_back.lock_stack_count = tempVgaBackLockStackCount;
	vga_back.lock_bit_stack =   tempVgaBackLockBitStack;
	if ((tempVgaFrontBufLocked) && (!vga_front.buf_locked))
		vga_front.lock_buf();
	if ((!tempVgaFrontBufLocked) && (vga_front.buf_locked))
		vga_front.unlock_buf();

	if ((tempVgaBackBufLocked) && (!vga_back.buf_locked))
		vga_back.lock_buf();
	if ((!tempVgaBackBufLocked) && (vga_back.buf_locked))
		vga_back.unlock_buf();

	// ##### begin Gilbert 30/10 #######//
	need_redraw_flag = 1;
	// ##### end Gilbert 30/10 #######//

	return rc;
}
//-------- End of function Sys::change_mode --------//


// ###### begin Gilbert 4/11 ######//
// ------ begin of function Sys::capture_screen ------//
//
void Sys::capture_screen()
{
   // NB: Increase this when allowing more decimals in the screenshot file, or when changing the screenshot filename
   enum {MAX_SCREENSHOT_FILENAME_LENGTH = 8};

   char full_path[MAX_PATH+1];
   int path_len;

   strcpy(full_path, dir_config);
   path_len = strlen(full_path);
   if (path_len + MAX_SCREENSHOT_FILENAME_LENGTH > MAX_PATH)
   {
      ERR("Path to the screenshots too long.\n");
      return;
   }

   String str("7K");

   int i;
   for( i=0 ; i<=99 ; i++ )
   {
      str  = "7K";

      if( i<10 )
         str += "0";

      str += i;
      str += ".BMP";

      strcpy(full_path + path_len, str);

      if( !misc.is_file_exist(full_path) )
         break;
   }

   if( i>99 )        // all file names from DWORLD00 to DWORLD99 have been occupied
      return;

//   if( sys.debug_session )    // in debug session, the buffer is not locked, we need to lock it for capturing the screen
   if( use_true_front )    // in triple buffer , the buffer is not locked, we need to lock it for capturing the screen
   {
      vga_true_front.lock_buf();
      vga_true_front.write_bmp_file(full_path);
      vga_true_front.unlock_buf();
   }
   else
   {
      vga_front.write_bmp_file(full_path);
   }

   //------ display msg --------//

	box.msg( text_game_menu.str_capture_screen((char *)str) );
}
// ------ end of function Sys::capture_screen ------//
// ###### end Gilbert 4/11 ######//
