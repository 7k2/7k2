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

// Filename    : OINGMENU.H
// Description : in-game menu (async version)

#include <OINGMENU.H>
#include <OVGA.H>
#include <OVGABUF.H>
#include <OMODEID.H>
#include <OCOLTBL.H>
#include <OSYS.H>
#include <OIMGRES.H>
#include <OMOUSE.H>
#include <OMOUSECR.H>
#include <KEY.H>
#include <OPOWER.H>
#include <OBOX.H>
#include <OREMOTE.H>
#include <OTUTOR.H>
#include <ONATIONA.H>
#include <OWORLDMT.H>
#include <OGAME.H>
#include <OOPTMENU.H>
#include <OFONT.H>
#include <OMUSIC.H>
#include <OC_EAST.H>
#include <OINFO.H>
#include <OT_GMENU.H>



enum { GAME_MENU_WIDTH  = 548,
       GAME_MENU_HEIGHT = 532  };

//enum { GAME_OPTION_WIDTH  = 170,
//       GAME_OPTION_HEIGHT = 34   };

enum { ING_OPTION = 0,
			ING_VIEW_HOTKEY,			
			ING_SAVE_GAME,
			ING_LOAD_GAME,
			ING_RESTART_GAME,
			ING_RETIRE_GAME,
			ING_MISSION_BRIEFING,
			ING_CAMPAIGN_CHAT,
			ING_QUIT_TO_MAIN_MENU,
			ING_QUIT_TO_WINDOWS,
			ING_CONTINUE_GAME
	};

//static char *menu_option_str[InGameMenu::GAME_OPTION_COUNT] = 
//{
//	"Options (O)",
//	"Save Game (S)",
//	"Load Game (L)",
//	"Restart Game",
//	NULL,			// retire (non-campaign) or retreat
//	"Mission Briefing",
//	"Campaign Cheat to Win",
//	"Quit to Main Menu",
//	"Quit to Windows",
//	"Continue Game",
//};

//char *menu_option_4_str[2] = 
//{
//	"Retire",
//	"Retreat",
//};

unsigned InGameMenu::menu_hot_key[InGameMenu::GAME_OPTION_COUNT] = {'o',0,'s','l', 0,0,0,0,0,0,KEY_ESC };

InGameMenu::InGameMenu()
{
   active_flag = 0;
	color_remap_table = NULL;
	background_bitmap = NULL;
}


InGameMenu::~InGameMenu()
{
	if( color_remap_table )
	{
		mem_del( color_remap_table );
		color_remap_table = NULL;
	}

	if( background_bitmap )
	{
		mem_del( background_bitmap );
		background_bitmap = NULL;
	}
}



void InGameMenu::set_active()
{
	active_flag = 1;

	// load palette and bitmap so no need to load every time

	if( !color_remap_table )
	{
		color_remap_table = (short *)mem_add( sizeof(*color_remap_table)*0x100 );

		ColorTable colorTable;
		{
			File palFile;
			{	// save stack space for str and palBuf
				String str = DIR_IMAGE;
				str += "CHOOSE3.COL";
				palFile.file_open(str);
			}
			{
				BYTE palBuf[0x100][3];
				palFile.file_seek(8);     				// bypass the header info
				palFile.file_read(palBuf, sizeof(palBuf));
				palFile.file_close();

				// ------- palette description -------------//

				PalDesc palBufDesc( palBuf, 3, 0x100, 8 );

				//-------- create color remap table ---------//

				colorTable.generate_table_fast( 0, palBufDesc, ColorTable::bright_func );
			}
		}

		memcpy( color_remap_table, colorTable.get_table(0), sizeof(*color_remap_table)*0x100 );
	}

	if( !background_bitmap )
	{
		File imageFile;
		String str = DIR_IMAGE;
		str += "CHOOSE3.ICN";
		imageFile.file_open(str);
		background_bitmap = (char *)mem_add( imageFile.file_size() );
		imageFile.file_read( background_bitmap, imageFile.file_size() );
	}
}


void InGameMenu::clear_active()
{
	if( background_bitmap )
	{
		mem_del( background_bitmap );
		background_bitmap = NULL;
	}

	if( color_remap_table )
	{
		mem_del( color_remap_table );
		color_remap_table = NULL;
	}

	active_flag = 0;
}




void InGameMenu::enter(char untilExitFlag)
{
   if( active_flag )
      return;

   refresh_flag = 1;
   // active_flag = 1;
	set_active();

   memset(game_menu_option_flag, 1, sizeof(game_menu_option_flag) );

   if( !nation_array.player_recno)
   {
      // when in observe mode
      game_menu_option_flag[ING_SAVE_GAME] = 0;    // disable save game
      game_menu_option_flag[ING_RETIRE_GAME] = 0;    // disable retire
		game_menu_option_flag[ING_MISSION_BRIEFING] = 0;    // disable missing brief
		game_menu_option_flag[ING_CAMPAIGN_CHAT] = 0;    // disable campaign chat
   }

   if( remote.is_enable() )
   {
      // when in when in multi-player mode,
      game_menu_option_flag[ING_LOAD_GAME] = 0;    // disable load game
      game_menu_option_flag[ING_RESTART_GAME] = 0;    // disable restart
      game_menu_option_flag[ING_RETIRE_GAME] = 0;    // disable retire
		game_menu_option_flag[ING_MISSION_BRIEFING] = 0;    // disable missing brief
		game_menu_option_flag[ING_CAMPAIGN_CHAT] = 0;    // disable campaign chat
   }

	// ------- in campaign -------//

	if( game.is_campaign_mode() )
	{
		//----- enable/disable retreat -----//

		short allowRetreatStageArray[] = { 4, 5, 9, 12, 14, 17, 21, 22, 23, 24, 0 };

		game_menu_option_flag[ING_RETIRE_GAME] = 0;

		for( int i=0 ; i<28 && allowRetreatStageArray[i] ; i++ )
		{
			if( game.campaign()->stage_id==allowRetreatStageArray[i] )
			{
				game_menu_option_flag[ING_RETIRE_GAME] = 1;
				break;
			}
		}

		//---- enable/disable cheat to win ----//

		if (game.campaign()->stage_id == 6 &&
			((CampaignEastWest*)game.campaign())->hero_unit_recno==0 )
		{
			game_menu_option_flag[ING_CAMPAIGN_CHAT] = 0;
		}
	}
	else
	{
		game_menu_option_flag[ING_RESTART_GAME] = 0;		// disable restart
//		menu_option_str[ING_RETIRE_GAME] = menu_option_4_str[0];		// retire
		game_menu_option_flag[ING_MISSION_BRIEFING] = 0;    // disable missing brief in non campaign mode
		game_menu_option_flag[ING_CAMPAIGN_CHAT] = 0;   // disable campaign chat in non campaign mode
	}

	if (game.game_mode == GAME_TUTORIAL)
	{		
		game_menu_option_flag[ING_VIEW_HOTKEY] = 0;    // disable save game
      
		if (tutor.selectively_detect_flag != 5 && 
			tutor.selectively_detect_flag != 0 &&
			tutor.selectively_detect_flag != 1)
		{
			game_menu_option_flag[ING_OPTION] = 0;
			game_menu_option_flag[ING_SAVE_GAME] = 0;
			game_menu_option_flag[ING_LOAD_GAME] = 0;
		}
	}				

   mouse_cursor.set_icon(CURSOR_NORMAL);
   mouse_cursor.set_frame(0);

   power.win_opened = 1;

   if( untilExitFlag )
   {
      while( is_active() )
      {
         sys.yield();
         mouse.get_event();

         // display on front buffer
         char useBackBuf = vga.use_back_buf;
         vga.use_front();
         disp();
         if(useBackBuf)
            vga.use_back();

         sys.blt_virtual_buf();
         music.yield();
         detect();
      }
   }
}

void InGameMenu::disp(int needRepaint)
{
   if( !active_flag )
      return;

	err_when( !color_remap_table || !background_bitmap );		// call set_active to load palette and bitmap

	int bx = (ZOOM_X1 + ZOOM_X2 - GAME_MENU_WIDTH) / 2;
	int by = (ZOOM_Y1 + ZOOM_Y2 - GAME_MENU_HEIGHT) / 2;

	err_when( bx < ZOOM_X1 );

   // since use back buffer, always refresh
   if( Vga::use_back_buf || needRepaint )
      refresh_flag = 1;

   if( refresh_flag )
   {
      int x1=bx+105;
		int x2=bx+GAME_MENU_WIDTH-105;
		int y=by+80;
	//	int ySpacing = font_bold_black.max_font_height + 10;
		int ySpacing = font_bold_black.max_font_height + 6;

//      if( Vga::use_back_buf )
//         image_interface.put_back( GAME_MENU_X1, GAME_MENU_Y1, "GAMEMENU" );
//      else
//         image_interface.put_front( GAME_MENU_X1, GAME_MENU_Y1, "GAMEMENU" );
		mouse.hide();
		vga.active_buf->put_bitmap_trans_remap(bx, by, background_bitmap, color_remap_table );

      for( int b = 0; b < GAME_OPTION_COUNT; ++b)
      {
         if( game_menu_option_flag[b])
         {
				String str;

				switch( b )
				{
				case ING_OPTION:
					str = text_game_menu.str_option();
					break;
				case ING_VIEW_HOTKEY:
					str = text_game_menu.str_view_hot_key();
					break;
				case ING_SAVE_GAME:
					str = text_game_menu.str_save_game();
					break;
				case ING_LOAD_GAME:
					str = text_game_menu.str_load_game();
					break;
				case ING_RESTART_GAME:
					str = text_game_menu.str_restart();
					break;
				case ING_RETIRE_GAME:
					str = text_game_menu.str_retire( game.is_campaign_mode() );
					break;
				case ING_MISSION_BRIEFING:
					str = text_game_menu.str_mission_briefing();
					break;
				case ING_CAMPAIGN_CHAT:
					str = text_game_menu.str_cheat_to_win();
					break;
				case ING_QUIT_TO_MAIN_MENU:
					str = text_game_menu.str_quit_main();
					break;
				case ING_QUIT_TO_WINDOWS:
					str = text_game_menu.str_quit_game();
					break;
				case ING_CONTINUE_GAME:
					str = text_game_menu.str_continue_game();
					break;
				default:
					err_here();
				}

				// add hot key
				if( menu_hot_key[b] >= ' ' )
				{
					char keyStr[2] = "?";
					keyStr[0] = (char) font_bold_black.upper_case(menu_hot_key[b]);
					str += " (";
					str += keyStr;
					str += ")";
				}

				font_bold_black.center_put( x1, y + ySpacing * b, x2, y + ySpacing * b + font_bold_black.max_font_height,
					str );
         }
      }

		mouse.show();

      refresh_flag = 0;
   }
}


int InGameMenu::detect()
{
   if( !active_flag )
      return 0;

	int bx = (ZOOM_X1 + ZOOM_X2 - GAME_MENU_WIDTH) / 2;
	int by = (ZOOM_Y1 + ZOOM_Y2 - GAME_MENU_HEIGHT) / 2;

   int x1=bx+105;
	int x2=bx+GAME_MENU_WIDTH-105;
	int y=by+80;
//	int ySpacing = font_bold_black.max_font_height + 10;
	int ySpacing = font_bold_black.max_font_height + 6;

   for( int b = 0; b < GAME_OPTION_COUNT; ++b)
   {
      if( game_menu_option_flag[b] == 1 &&
         (menu_hot_key[b] && mouse.key_code == menu_hot_key[b] ||
         mouse.single_click( x1, y + ySpacing * b, x2, y + ySpacing * b + font_bold_black.max_font_height )) )
         break;

      if( b == GAME_OPTION_COUNT-1 &&    // assume last option is 'continue'
         (mouse.any_click(1) || mouse.key_code==KEY_ESC) )
         break;
   }

   if( b >= GAME_OPTION_COUNT )
      return 0;

   //------ display the pressed down button -----//

	String str;

	switch( b )
	{
	case ING_OPTION:
		str = text_game_menu.str_option();
		break;
	case ING_VIEW_HOTKEY:
		str = text_game_menu.str_view_hot_key();
		break;		
	case ING_SAVE_GAME:
		str = text_game_menu.str_save_game();
		break;
	case ING_LOAD_GAME:
		str = text_game_menu.str_load_game();
		break;
	case ING_RESTART_GAME:
		str = text_game_menu.str_restart();
		break;
	case ING_RETIRE_GAME:
		str = text_game_menu.str_retire( game.is_campaign_mode() );
		break;
	case ING_MISSION_BRIEFING:
		str = text_game_menu.str_mission_briefing();
		break;
	case ING_CAMPAIGN_CHAT:
		str = text_game_menu.str_cheat_to_win();
		break;
	case ING_QUIT_TO_MAIN_MENU:
		str = text_game_menu.str_quit_main();
		break;
	case ING_QUIT_TO_WINDOWS:
		str = text_game_menu.str_quit_game();
		break;
	case ING_CONTINUE_GAME:
		str = text_game_menu.str_continue_game();
		break;
	default:
		err_here();
	}

	// add hot key
	if( menu_hot_key[b] >= ' ' )
	{
		char keyStr[2] = "?";
		keyStr[0] = (char) font_bold_red.upper_case(menu_hot_key[b]);
		str += " (";
		str += keyStr;
		str += ")";
	}

	font_bold_red.center_put( x1, y + ySpacing * b, x2, y + ySpacing * b + font_bold_black.max_font_height,
		str );

#ifndef NO_REAL_TIME_UPDATE
	if( !vga.use_back_buf )
		sys.blt_virtual_buf();
#endif

   while( mouse.left_press )  // holding down the button
   {
      sys.yield();
      mouse.get_event();
   }

   //--------- run the option -------//

   exit(0);		// member function exit

	switch(b)
	{
	case ING_OPTION:     // options
		option_menu.enter(!remote.is_enable());
		break;

	case ING_VIEW_HOTKEY:
		info.display_hot_key = 1;
		break;
	
	case ING_SAVE_GAME:     // save game
		sys.save_game();
		break;

	case ING_LOAD_GAME:     // load game
		sys.load_game();
		break;

	case ING_RESTART_GAME:     // restart
		// tutor.select_run_tutor(1);

		if( game.is_campaign_mode()
			// && box.ask( "Do you really want to restart?", "Yes", "No", 175,320) )
			&& box.ask( text_game_menu.str_ask_restart(), 
				text_game_menu.str_no_yes(1),	// "Yes"
				text_game_menu.str_no_yes(0),	// "No"
				175,320) )
		{
			sys.signal_exit_flag = 3;
			game.campaign()->replay_stage_flag = 1;
		}
		break;

	case ING_RETIRE_GAME:     // retire
		if( nation_array.player_recno )     // only when the player's kingdom still exists
		{
			if( box.ask( text_game_menu.str_ask_retire(game.is_campaign_mode()),
				text_game_menu.str_no_yes(1), text_game_menu.str_no_yes(0), 175, 320) )
			{
				if( remote.is_enable() )
				{
					// BUGHERE : message will not be sent out
					short *shortPtr = (short *)remote.new_send_queue_msg( MSG_PLAYER_QUIT, 2*sizeof(short));
					shortPtr[0] = nation_array.player_recno;
					shortPtr[1] = 1;     // retire
				}
				game.game_end(0, 0, 0, 1);          // 1 - retire
			}
		}
		break;

	case ING_MISSION_BRIEFING:     // mission brief for campaign
	{
		info.display_campaign_mission_briefing = 1;
		break;
	}

	case ING_CAMPAIGN_CHAT:
	{
		if( game.is_campaign_mode()
			 && box.ask( text_game_menu.str_ask_cheat_win(),
				text_game_menu.str_no_yes(1), text_game_menu.str_no_yes(0), 120, 320) )
		{
			game.result_win_condition_recno = 0;
			game.result_win_condition_id = game.win_condition[game.result_win_condition_recno].condition_id;
			game.campaign()->cheat_to_win_flag = 1;
			game.game_end(nation_array.player_recno);
		}
		break;
	}

	case ING_QUIT_TO_MAIN_MENU:     // quit to main menu
	{
		int boxX1;

		boxX1 = 115;

		if( !nation_array.player_recno ||
			box.ask( text_game_menu.str_ask_quit_main(),
			text_game_menu.str_no_yes(1), text_game_menu.str_no_yes(0), boxX1, 350 ) )
		{
			if( remote.is_enable() && nation_array.player_recno )
			{
				// BUGHERE : message will not be sent out
				short *shortPtr = (short *)remote.new_send_queue_msg( MSG_PLAYER_QUIT, 2*sizeof(short));
				shortPtr[0] = nation_array.player_recno;
				shortPtr[1] = 0;     // not retire
			}
			sys.signal_exit_flag = 2;
		}
		break;
	}

	case ING_QUIT_TO_WINDOWS:     // quit to Windows
		if( !nation_array.player_recno ||
			box.ask( text_game_menu.str_ask_quit_game(),
			text_game_menu.str_no_yes(1), text_game_menu.str_no_yes(0), 130, 400 ) )
		{
			if( remote.is_enable() && nation_array.player_recno )
			{
				// BUGHERE : message will not be sent out
				short *shortPtr = (short *)remote.new_send_queue_msg( MSG_PLAYER_QUIT, 2*sizeof(short));
				shortPtr[0] = nation_array.player_recno;
				shortPtr[1] = 1;     // retire
			}
			sys.signal_exit_flag = 1;
		}
		break;
	}

   return 1;
}


void InGameMenu::exit(int action)
{
   power.win_opened = 0;
   // active_flag = 0;
	clear_active();
}


void InGameMenu::abort()
{
   power.win_opened = 0;
   // active_flag = 0;
	clear_active();
}
