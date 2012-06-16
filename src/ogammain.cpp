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

//Filename    : OGAMMAIN.CPP
//Description : Main Game Object - Main menu

#include <ogame.h>
#include <ovga.h>
#include <vga_util.h>
#include <omodeid.h>
#include <oimgres.h>
#include <osys.h>
#include <omouse.h>
#include <omousecr.h>
#include <ofont.h>
#include <otutor.h>
#include <obutton.h>
#include <obattle.h>
#include <ogfile.h>
#include <omusic.h>
#include <oconfig.h>
#include <ovgalock.h>
#include <oc_east.h>
// #include <oc_tutor.h>
#include <oprofile.h>
#include <ogammenu.h>
#include <obox.h>
#include <odplay.h>
#include <ot_gmenu.h>
#include <ocoltbl.h>

#undef DISABLE_SINGLE_PLAYER_NEW_GAME
#undef DISABLE_SCENARIO_EDITOR

#ifdef DEMO
#define DISABLE_MULTI_PLAYER
#define DISABLE_SINGLE_PLAYER_NEW_GAME
#define DISABLE_SCENARIO_EDITOR
#endif

// define the macro to disable single player menu
//#define DISABLE_SINGLE_PLAYER

//----------- Define structure ---------//

struct OptionInfo
{
	short x1, y1, x2, y2;
};

// -------- define constant ----------//

enum { SWORD1_X = 258, SWORD1_Y = 194 };


//-------- Begin of function Game::run_main_menu_option --------//
//
// Run the selected game option.
//
void Game::run_main_menu_option(int optionId)
{
	if( optionId==0 )
	{
		player_profile.register_menu();
	}

	//------- Single Player Game -------//

	if( optionId==1 )
	{
#ifndef DISABLE_SINGLE_PLAYER
		game_mode = GAME_SINGLE_PLAYER;
		single_player_menu();
#endif
	}

	//-------- Multiplayer Game ----------//

	if( optionId==2 )
	{
#ifndef DISABLE_MULTI_PLAYER
		game_mode = GAME_MULTI_PLAYER;
		multi_player_menu(NULL);
#endif
	}

	//----------- scenario editor -----------//

	if( optionId==3 )
	{
		// game_mode = GAME_ENCYCLOPEDIA;
		// view_encyclopedia();
#ifndef DISABLE_SCENARIO_EDITOR
		game_mode = GAME_SCENARIO_EDITOR;
		++sys.scenario_cheat_flag;
		scenario_editor_menu();
		--sys.scenario_cheat_flag;
#endif
	}

	//----------- Hall of Fame -----------//

	if( optionId==4 )
	{
		game_file_array.disp_hall_of_fame(0);
		game_file_array.disp_hall_of_fame(1);
	}

	//------------- Credits -----------//
	if( optionId==5 )
	{
		game_mode = GAME_CREDITS;
		view_credits();
	}

	if( optionId==6 )		// go to home page
	{
		const char *shortcutFilename = "7k2home.url";

		if( !m.is_file_exist(shortcutFilename) )
		{
			// create 7k2home.url file

			char fileStr[] = "[InternetShortcut]\r\nURL=http://www.enlight.com/7k2\r\n";

			File urlFile;
			urlFile.file_create(shortcutFilename);
			urlFile.file_write( fileStr, strlen(fileStr) );
			urlFile.file_close();
		}

		if( m.is_file_exist("7k2home.url") )
		{
			HINSTANCE hinst = ShellExecute( sys.main_hwnd, "open", shortcutFilename, NULL, "", SW_SHOWNORMAL );
		}
	}

	if( optionId==7 )
	{
		sys.signal_exit_flag = 1;
	}
}
//---------- End of function Game::run_main_menu_option ---------//

//-------- Begin of static function disp_version --------//
//
void Game::disp_version()
{
	//----------- display version string --------//

	String str;

	// ####### begin Gilbert 5/6 ########//
//	str  = "Version ";
//	str += GAME_VERSION_STR;
//	#ifdef DEMO
//		str = "Demo Version";
//	#endif
//	#ifdef BETA
//		str = "This is a Beta version. Unauthorized distribution of this Beta is strictly prohibited.";
//	#endif

#if(defined(BETA))
//	str = "This is a Beta version. Unauthorized distribution of this Beta is strictly prohibited.";
	str = text_game_menu.str_beta_version();
#elif(defined(DEMO))
//	str = "Demo Version";
//	str = text_game_menu.str_demo_version();
	str = "";
#else
//	str  = "Version ";
//	str += GAME_VERSION_STR;
	str = text_game_menu.str_version( GAME_VERSION_STR );
#endif
	// ####### end Gilbert 5/6 ########//

	if( str.len() > 40 )
		font_san.center_put( 0, VGA_HEIGHT-20, VGA_WIDTH-1, VGA_HEIGHT-1, str );
	else
		font_zoom.put( VGA_WIDTH-100, 73, str );
}
//---------- End of function Game::disp_version ---------//


//--------- Begin of function Game::disp_gen_game_status ---------//
//
// <int> addStep - the steps to be added to the current gen game
//						 progress.
//      				 (pass 0 for starting the generating game process.
//
void Game::disp_gen_game_status(int addStep)
{
	// #### begin Gilbert 14/12 ######//
	{
		VgaFrontLock vgaLock;

		MSG msg;
		while (PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE) > 0)
		{
			// do not get WM_QUIT
			if (!GetMessage( &msg, NULL, 0, 0))
			{
				sys.signal_exit_flag = 1;
				// BUGHERE : vga_front is unlocked
				return;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	// #### end Gilbert 14/12 ######//

	int POPUP_WINDOW_WIDTH;
	int POPUP_WINDOW_HEIGHT;

	if (game.is_campaign_mode())
	{
		POPUP_WINDOW_WIDTH = 444;
		POPUP_WINDOW_HEIGHT = 275;
	}
	else
	{
#if(!defined(GERMAN) && !defined(FRENCH) && !defined(SPANISH) && !defined(ITALIAN))	// US
		POPUP_WINDOW_WIDTH = 535;
		POPUP_WINDOW_HEIGHT = 386;
#else		
		POPUP_WINDOW_WIDTH = 444;
		POPUP_WINDOW_HEIGHT = 275;
#endif
	}	

//	const POPUP_WINDOW_WIDTH = 266;
//	const POPUP_WINDOW_HEIGHT = 149;
//	const POPUP_WINDOW_WIDTH = 535;
//	const POPUP_WINDOW_HEIGHT = 386;
	
	const int POPUP_WINDOW_X1 = (vga_front.buf_width() - POPUP_WINDOW_WIDTH) / 2;
	const int POPUP_WINDOW_Y1 = (vga_front.buf_height() - POPUP_WINDOW_HEIGHT) / 2;

	const int BAR_X1 = POPUP_WINDOW_X1 + 105;
	const int BAR_Y1 = POPUP_WINDOW_Y1 + 198;

	static int genGameProgress=0;
	
	//	int hasLocked=0;
	//	if( !vga_front.buf_locked )		// lock buffer 
	//	{
	//		vga_front.temp_lock();
	//		hasLocked = 1;
	//	}
	VgaFrontReLock vgaReLock;

	// ------- draw status background ------ //

	if( addStep == 0 || sys.need_redraw_flag )
	{
		if (game.is_campaign_mode())
			image_menu.put_front(POPUP_WINDOW_X1, POPUP_WINDOW_Y1, "NEWWORLD");
		sys.need_redraw_flag = 0;
	}


	//---- if addStep == 0, start the game generation process ----//

	if( addStep == 0 )
	{
		genGameProgress = 0;
		if (game.is_campaign_mode())
			image_menu.put_front(POPUP_WINDOW_X1, POPUP_WINDOW_Y1, "NEWWORLD");
		return;
	}

	//------------------------------------------//

	genGameProgress += addStep;

	int hasLocked=0;

	if( vga_front.vptr_dd_buf )
	{
		if (game.is_campaign_mode())
		{
			short*	progressBitmap =NULL;
			int size = genGameProgress *237 / 100;

			if (size > 237)
				size = 237;
			
			progressBitmap = (short *)mem_add( BitmapW::size(size, 16) );	
			
			vga.active_buf->put_bitmap( BAR_X1, BAR_Y1, image_menu.read("PROGRESS") );
			vga.active_buf->read_bitmapW( BAR_X1, BAR_Y1, BAR_X1 +size -1, BAR_Y1 +16-1, progressBitmap );
						
			short*	colorRemapTable = NULL;
			char*		backgroundBitmap = NULL;
			colorRemapTable = (short *)mem_add( sizeof(*colorRemapTable)*0x100 );
			ColorTable colorTable;
			{
				File palFile;
				{
					String str(DIR_IMAGE);
					str += "NEWWORLD.COL";
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
				memcpy( colorRemapTable, colorTable.get_table(0), sizeof(*colorRemapTable)*0x100 );
			}
			
			File imageFile;
			String str(DIR_IMAGE);
			str += "NEWWORLD.ICN";
			imageFile.file_open(str);
			backgroundBitmap = (char *)mem_add( imageFile.file_size() );
			imageFile.file_read( backgroundBitmap, imageFile.file_size() );
			vga.active_buf->put_bitmap_trans_remap(POPUP_WINDOW_X1, POPUP_WINDOW_Y1, backgroundBitmap, colorRemapTable );

			vga.active_buf->put_bitmapW_trans( BAR_X1, BAR_Y1, progressBitmap );
						
			mem_del( backgroundBitmap );
			mem_del( colorRemapTable );
			mem_del( progressBitmap );				
		}
		else
		{
#if(defined(GERMAN) || defined(FRENCH) || defined(SPANISH) || defined(ITALIAN))
			// prepare palette
			ColorTable colorTable;
			{
				String str = DIR_IMAGE;
				str += "NW.COL";
				PalDescFile palBufDesc( str, 8, 3, 0x100, 8 );	// 8 byte header
				colorTable.generate_table_fast( 0, palBufDesc, ColorTable::bright_func );
			}
			short* colorRemapTable = (short *)colorTable.get_table(0);

			// put background bitmap
			{
				File imageFile;
				String str = DIR_IMAGE;
				str += "NW.ICN";
				imageFile.file_open(str);
				vga.active_buf->put_large_bitmap_trans( POPUP_WINDOW_X1, POPUP_WINDOW_Y1,
					&imageFile, colorRemapTable );
			}

			// put bar
			int size = genGameProgress *237 / 100;
			if (size > 237)
				size = 237;
			if( size > 0 )
			{
				vga.active_buf->put_bitmap_area_trans( BAR_X1, BAR_Y1, image_menu.read("PROGRESS"),
					0, 0, size -1, 16-1 );
			}
#else
			char fileName[] = "NW_00";
			fileName[3] = '0' + (char) ((((genGameProgress) /5) +1) /10);
			fileName[4] = '0' + (char) ((((genGameProgress) /5) +1) %10);
			image_menu.put_front(POPUP_WINDOW_X1, POPUP_WINDOW_Y1, fileName);
#endif
		}
		sys.blt_virtual_buf();
	}
	// ####### end Gilbert 10/3 #########//
}
//---------- End of function Game::disp_gen_game_status ---------//

// ----- define const for flag of refreshFlag ------//

#define MMOPTION_PAGE        0x40000000
#define MMOPTION_RESOLUTION  0x00000001
#define MMOPTION_ALL         0x7fffffff

void Game::main_menu()
{
	int refreshFlag = MMOPTION_ALL;

	char optionFlag[5] = 
	{
		1, 1, 1, 1, 1,
	};

	// ----- disable some option -------//

	// test sys.game_version, skip single player and scenario editor
	optionFlag[0] = optionFlag[2] =
#if(defined(DISABLE_SINGLE_PLAYER))
		0;
#elif(defined(DEBUG))
		1;
#elif(defined(BETA))
		1;
#else
		(sys.game_version == VERSION_MULTIPLAYER_ONLY) ? 0 : 1;
#endif


#ifdef DISABLE_MULTI_PLAYER
	// disable multiplayer game, Game::multi_player_menu is disabled
	optionFlag[1] = 0;
#endif

#ifdef DISABLE_SCENARIO_EDITOR
	optionFlag[2] = 0;
#endif

	int emptyProfileTesting = 1;

	mouse_cursor.set_icon(CURSOR_NORMAL);
	vga_front.bar(0,0,VGA_WIDTH-1,VGA_HEIGHT-1,V_BLACK);

	unsigned long lastRedrawTime = m.get_time();

	{
		VgaFrontLock vgaLock;

		while(1)
		{
			MSG msg;
			if (PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE))
			{
				if (!GetMessage( &msg, NULL, 0, 0))
				{
					sys.signal_exit_flag = 1;
					// BUGHERE : vga_front is unlocked
					return;
				}
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				continue;
			}
			else if( sys.paused_flag || !sys.active_flag )
			{
				WaitMessage();
				continue;
			}
			if( sys.need_redraw_flag || m.get_time() - lastRedrawTime > 8000 )
			{
				refreshFlag = MMOPTION_ALL;
				sys.need_redraw_flag = 0;
				lastRedrawTime = m.get_time();	// redraw every 8 sec. Such that if background erased at the beginning, redraw
			}

			VgaFrontReLock vgaReLock;

			// -------- check auto load game request ------//

			if( auto_load_file_name )
			{
				refreshFlag = 0;
			}

			// -------- display ----------//

			game_mode = GAME_PREGAME;

			if( refreshFlag )
			{
				if( refreshFlag & MMOPTION_PAGE )
				{
					vga.use_back();
					vga_util.disp_image_file("M_MAIN");

				//	// ------ display copyright message --------//

				//	font_zoom.right_put( 755, 62, "(c) Copyright 1999 by Enlight Software and Trevor Chan" );

					// ------ display button -------//

					if( player_profile.is_registered() )
					{
						if( optionFlag[0] > 0 )		// single player
							font_thin_black.center_put_paragraph(
								BUTTON1_X1, BUTTON1_Y1, BUTTON1_X2, BUTTON1_Y2, 
								text_game_menu.str_single_player(), 0);
						if( optionFlag[1] > 0 )		// multi player
							font_thin_black.center_put_paragraph(
								BUTTON2_X1, BUTTON2_Y1, BUTTON2_X2, BUTTON2_Y2, 
								text_game_menu.str_multi_player(), 0);
						if( optionFlag[2] > 0 )		// scenario editor
							font_thin_black.center_put_paragraph(
								BUTTON3_X1, BUTTON3_Y1, BUTTON3_X2, BUTTON3_Y2, 
								text_game_menu.str_scenario_editor(), 0);
					}
					if( optionFlag[3] > 0 )
						font_thin_black.center_put_paragraph(
							BUTTON4_X1, BUTTON4_Y1, BUTTON4_X2, BUTTON4_Y2, 
							text_game_menu.str_hall_of_fame(), 0);
					if( optionFlag[4] > 0 )
						font_thin_black.center_put_paragraph(
							BUTTON5_X1, BUTTON5_Y1, BUTTON5_X2, BUTTON5_Y2, 
							text_game_menu.str_credits(), 0);
					font_thin_black.center_put_paragraph(
						BUTTON6_X1, BUTTON6_Y1, BUTTON6_X2, BUTTON6_Y2, 
						text_game_menu.str_player_register(), 0);
					font_thin_black.center_put_paragraph(
						BUTTON8_X1, BUTTON8_Y1, BUTTON8_X2, BUTTON8_Y2, 
						text_game_menu.str_quit(), 0);
					font_thin_black.center_put_paragraph(
						BUTTONA_X1, BUTTONA_Y1, BUTTONA_X2, BUTTONA_Y2, 
						text_game_menu.str_web_site(), 0);
					vga_util.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
					vga.use_front();

					disp_version();
				}

				if( refreshFlag & MMOPTION_RESOLUTION )
				{
					String resStr;

					if( !DisplayModeInfo::get_display_info(config.display_mode_id) )
						config.display_mode_id = MODE_ID_DEFAULT;

					resStr  = DisplayModeInfo::get_display_info(config.display_mode_id)->screen_width;
					resStr += "X";
					resStr += DisplayModeInfo::get_display_info(config.display_mode_id)->screen_height;

					vga_util.blt_buf( BUTTON9_X1, BUTTON9_Y1, BUTTON9_X2, BUTTON9_Y2, 0);
					font_thin_black.center_put( BUTTON9_X1, BUTTON9_Y1, BUTTON9_X2, BUTTON9_Y2, resStr );

					disp_version();
				}

				refreshFlag = 0;
			}

			sys.blt_virtual_buf();		// blt the virtual front buffer to the screen
			sys.yield();
			mouse.get_event();
			if( config.music_flag )
			{
				if( !music.is_playing(3) )
					music.play(3, sys.cdrom_drive ? MUSIC_CD_THEN_WAV : 0 );
			}
			else
			{
				music.stop();
			}

			sys.signal_exit_flag = 0;

			// -------- auto load game on request --------//

			if( auto_load_file_name )
			{
				int rc = game_file.load_game(NULL, auto_load_file_name);		// auto_load_file_name has the full path, no need to give path
				clear_load_game_in_main_menu();
				if( rc > 0 )
				{
					battle.run_loaded();
					deinit_all();
				}
				{
					char signalExitFlagBackup = sys.signal_exit_flag;
					sys.signal_exit_flag = 2;
					game.deinit();   // game.deinit() is needed if game_file_array.menu fails
					sys.signal_exit_flag = signalExitFlagBackup;
				}

				refreshFlag = MMOPTION_ALL;
				continue;
			}

			// ---------- detect new user ---------//

			if( emptyProfileTesting )		// do this testing only once
			{
				emptyProfileTesting = 0;

				if( PlayerProfile::load_count_profiles( NULL, 1) == 0 )		// maxLoad=1 for fast testing
				{
					box.msg( text_game_menu.str_first_welcome() );

					// run_main_menu_option(0);			don't call run_main_menu_option, we can't get it detect new profile
					if( player_profile.register_menu() == 1	// return 1 = new profile created
						&& player_profile.is_registered() 		// check is_registered for safety
						&& box.ask(text_game_menu.str_first_training()) )		// ask continue training
					{
						// ##### patch begin Gilbert 4/10 #####//
						game_file_array.init( player_profile.save_game_path(NULL), "*.SAV" );	// necessary to set the path and extension
						// ##### patch end Gilbert 4/10 #####//
						tutor.building_size = 2;	// default small building size
						tutor.run( 2, 0 );			// 2 - is the first fryhtan training tutorial id, 0 - not inGameCall
					}
				}

				if( sys.signal_exit_flag == 1 )
					break;
				else
					continue;
			}

			// -------- detect --------//

			if( player_profile.is_registered() && optionFlag[0] > 0
				&& mouse.single_click(BUTTON1_X1, BUTTON1_Y1, BUTTON1_X2, BUTTON1_Y2) )
			{
				run_main_menu_option(1);		// single player
				refreshFlag = MMOPTION_ALL;
			}
			else if( player_profile.is_registered() && optionFlag[1] > 0
				&& mouse.single_click(BUTTON2_X1, BUTTON2_Y1, BUTTON2_X2, BUTTON2_Y2) )
			{
				run_main_menu_option(2);		// multi player
				refreshFlag = MMOPTION_ALL;
			}
			else if( player_profile.is_registered() && optionFlag[2] > 0
				&& mouse.single_click(BUTTON3_X1, BUTTON3_Y1, BUTTON3_X2, BUTTON3_Y2) )
			{
				run_main_menu_option(3);		// scenario editor
				refreshFlag = MMOPTION_ALL;
			}
			else if( optionFlag[3] > 0 
				&& mouse.single_click( BUTTON4_X1, BUTTON4_Y1, BUTTON4_X2, BUTTON4_Y2) )
			{
				run_main_menu_option(4);		// hall of fame
				refreshFlag = MMOPTION_ALL;
			}
			else if( optionFlag[4] > 0
				&& mouse.single_click( BUTTON5_X1, BUTTON5_Y1, BUTTON5_X2, BUTTON5_Y2) )
			{
				run_main_menu_option(5);		// credits
				refreshFlag = MMOPTION_ALL;
			}
			else if( mouse.single_click(BUTTON6_X1, BUTTON6_Y1, BUTTON6_X2, BUTTON6_Y2) )
			{
				run_main_menu_option(0);		// profile
				refreshFlag = MMOPTION_ALL;
			}
			else if( mouse.single_click( BUTTON8_X1, BUTTON8_Y1, BUTTON8_X2, BUTTON8_Y2) )
			{
				// quit
				break;
			}
			else if( mouse.single_click( BUTTON9_X1, BUTTON9_Y1, BUTTON9_X2, BUTTON9_Y2) )
			{
				if( DisplayModeInfo::get_display_info(config.display_mode_id+1) )
					++config.display_mode_id;
				else
					config.display_mode_id = 0;
				refreshFlag |= MMOPTION_RESOLUTION;
			}
			else if( mouse.single_click( BUTTONA_X1, BUTTONA_Y1, BUTTONA_X2, BUTTONA_Y2) )
			{
				run_main_menu_option(6);		// home page
				refreshFlag = MMOPTION_ALL;
			}

			// ----- check signal exit flag -------//

			if( sys.signal_exit_flag == 1 )
				break;

		}	// end while
	}	// end the scope of vgaLock

//	#ifdef DEMO
//		demo_disp_ad_page();
//	#else
//		//---- display game end advertising page ----//
//		vga_util.disp_image_file("MAINMENU");
//		mouse.wait_press(60);
//		vga_util.finish_disp_image_file();
//	#endif
}


#define SPOPTION_PAGE        0x40000000
#define SPOPTION_ALL         0x7fffffff

#ifndef DISABLE_SINGLE_PLAYER
//---------- Begin of function Game::single_player_menu ----------//
//
void Game::single_player_menu()
{
	int refreshFlag = SPOPTION_ALL;

#ifdef DEMO
	char optionFlag[5] = { 1, 0, 0, 1, 1, };
#else
	char optionFlag[5] = { 1, 1, 1, 1, 1, };
#endif

	mouse_cursor.set_icon(CURSOR_NORMAL);

#ifdef DISABLE_SINGLE_PLAYER_NEW_GAME
	optionFlag[1] = 0;		// disable new campaign
	optionFlag[2] = 0;		// disable new single game
#endif
	optionFlag[4] = select_run_scenario(1) > 0;		// 1 - count number of scenario

	{
		VgaFrontLock vgaLock;

		while(1)
		{
			MSG msg;
			if (PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE))
			{
				if (!GetMessage( &msg, NULL, 0, 0))
				{
					sys.signal_exit_flag = 1;
					// BUGHERE : vga_front is unlocked
					return;
				}
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				continue;
			}
			else if( sys.paused_flag || !sys.active_flag )
			{
				WaitMessage();
				continue;
			}
			if( sys.need_redraw_flag )
			{
				refreshFlag = SPOPTION_ALL;
				sys.need_redraw_flag = 0;
			}

			VgaFrontReLock vgaReLock;

			// -------- display ----------//

			if( refreshFlag )
			{
				if( refreshFlag & SPOPTION_PAGE )
				{
					vga.use_back();
					vga_util.disp_image_file("M_MAIN");

					// ------ display button ------//

					if( optionFlag[0] )
						font_thin_black.center_put_paragraph(
							BUTTON1_X1, BUTTON1_Y1, BUTTON1_X2, BUTTON1_Y2, 
							text_game_menu.str_training(), 0 );
					if( optionFlag[1] )
						font_thin_black.center_put_paragraph(
							BUTTON2_X1, BUTTON2_Y1, BUTTON2_X2, BUTTON2_Y2, 
							text_game_menu.str_new_campaign(), 0 );
					if( optionFlag[2] )
						font_thin_black.center_put_paragraph(
							BUTTON3_X1, BUTTON3_Y1, BUTTON3_X2, BUTTON3_Y2, 
							text_game_menu.str_new_single_game(), 0 );
					if( optionFlag[3] )
						font_thin_black.center_put_paragraph(
							BUTTON4_X1, BUTTON4_Y1, BUTTON4_X2, BUTTON4_Y2, 
							text_game_menu.str_load_game(), 0 );
					if( optionFlag[4] )
						font_thin_black.center_put_paragraph(
							BUTTON5_X1, BUTTON5_Y1, BUTTON5_X2, BUTTON5_Y2, 
							text_game_menu.str_load_scenario(), 0 );
					font_thin_black.center_put_paragraph(
						BUTTON8_X1, BUTTON8_Y1, BUTTON8_X2, BUTTON8_Y2, 
						text_game_menu.str_cancel(), 0 );

					vga_util.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
					vga.use_front();
				}

				refreshFlag = 0;
			}

			sys.blt_virtual_buf();		// blt the virtual front buffer to the screen
			sys.yield();
			mouse.get_event();
			if( config.music_flag )
			{
				if( !music.is_playing(3) )
					music.play(3, sys.cdrom_drive ? MUSIC_CD_THEN_WAV : 0 );
			}
			else
			{
				music.stop();
			}
			// --------- detect -------//

			// detect training
			if( optionFlag[0] && mouse.single_click(BUTTON1_X1, BUTTON1_Y1, BUTTON1_X2, BUTTON1_Y2) )
			{
				game_file_array.init( player_profile.save_game_path(NULL), "*.SAV" );	// necessary to set the path and extension

				tutor.select_run_tutor(0);
				break;
			}

			// detect new campaign
			else if( optionFlag[1] && mouse.single_click( BUTTON2_X1, BUTTON2_Y1, BUTTON2_X2, BUTTON2_Y2) )
			{
				game_file_array.init( player_profile.save_game_path(NULL), "*.SAV" );	// necessary to set the path and extension

				run_campaign();
				break;
			}

			// detect new single game
			else if( optionFlag[2] && mouse.single_click( BUTTON3_X1, BUTTON3_Y1, BUTTON3_X2, BUTTON3_Y2) )
			{
				game_file_array.init( player_profile.save_game_path(NULL), "*.SAV" );	// necessary to set the path and extension

				single_player_game(0);
				break;
			}

			// detect load game
			else if( optionFlag[3] && mouse.single_click( BUTTON4_X1, BUTTON4_Y1, BUTTON4_X2, BUTTON4_Y2) )
			{
				game_file_array.init( player_profile.save_game_path(NULL), "*.SAV" );	// necessary to set the path and extension

				// ##### begin Gilbert 20/1 #######//
				// if( game_file_array.menu(2) == 1)
				if( game_file_array.menu(3) == 1)
				// ##### begin Gilbert 20/1 #######//
				{
					battle.run_loaded();
					deinit_all();
				}
				{
					char signalExitFlagBackup = sys.signal_exit_flag;
					sys.signal_exit_flag = 2;
					game.deinit();   // game.deinit() is needed if game_file_array.menu fails
					sys.signal_exit_flag = signalExitFlagBackup;
				}
				break;
			}

			// run scenario
			else if( optionFlag[4] && mouse.single_click(BUTTON5_X1, BUTTON5_Y1, BUTTON5_X2, BUTTON5_Y2) )
			{
				game_file_array.init( player_profile.save_game_path(NULL), "*.SAV" );	// necessary to set the path and extension

				select_run_scenario();
				break;
			}

			// cancel
			else if( mouse.single_click(BUTTON8_X1, BUTTON8_Y1, BUTTON8_X2, BUTTON8_Y2) )
			{
				break;
			}

		}	// end while
	}	// end scope of vgaLock
}
//---------- End of function Game::single_player_menu ----------//
#endif

#ifndef DISABLE_SCENARIO_EDITOR
//---------- Begin of function Game::scenario_editor_menu ----------//
//
void Game::scenario_editor_menu()
{
	int refreshFlag = SPOPTION_ALL;

	mouse_cursor.set_icon(CURSOR_NORMAL);

	{
		VgaFrontLock vgaLock;

		while(1)
		{
			MSG msg;
			if (PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE))
			{
				if (!GetMessage( &msg, NULL, 0, 0))
				{
					sys.signal_exit_flag = 1;
					// BUGHERE : vga_front is unlocked
					return;
				}
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				continue;
			}
			else if( sys.paused_flag || !sys.active_flag )
			{
				WaitMessage();
				continue;
			}
			if( sys.need_redraw_flag )
			{
				refreshFlag = SPOPTION_ALL;
				sys.need_redraw_flag = 0;
			}

			VgaFrontReLock vgaReLock;

			// -------- display ----------//

			if( refreshFlag )
			{
				if( refreshFlag & SPOPTION_PAGE )
				{
					vga.use_back();
					vga_util.disp_image_file("M_MAIN");

					// ------ display button ------//

					font_thin_black.center_put_paragraph(
						BUTTON2_X1, BUTTON2_Y1, BUTTON2_X2, BUTTON2_Y2, 
						text_game_menu.str_new_game(), 0 );
					font_thin_black.center_put_paragraph(
						BUTTON4_X1, BUTTON4_Y1, BUTTON4_X2, BUTTON4_Y2, 
						text_game_menu.str_load_game(), 0 );
					font_thin_black.center_put_paragraph(
						BUTTON8_X1, BUTTON8_Y1, BUTTON8_X2, BUTTON8_Y2, 
						text_game_menu.str_cancel(), 0 );

					vga_util.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
					vga.use_front();
				}

				refreshFlag = 0;
			}

			sys.blt_virtual_buf();		// blt the virtual front buffer to the screen
			sys.yield();
			mouse.get_event();
			if( config.music_flag )
			{
				if( !music.is_playing(3) )
					music.play(3, sys.cdrom_drive ? MUSIC_CD_THEN_WAV : 0 );
			}
			else
			{
				music.stop();
			}

			// --------- detect -------//

			// detect new 
			if( mouse.single_click( BUTTON2_X1, BUTTON2_Y1, BUTTON2_X2, BUTTON2_Y2) )
			{
				game_file_array.init( DIR_SCENARIO, "*.SCN" );	// necessary to set the path and extension

				single_player_game(0);
				break;
			}

			// detect load game
			else if( mouse.single_click( BUTTON4_X1, BUTTON4_Y1, BUTTON4_X2, BUTTON4_Y2) )
			{
				err_when( MAX_SCENARIO_PATH <= 1 );
				game_file_array.init( DIR_SCENARIO, "*.SCN" );	// necessary to set the path and extension

				// ##### begin Gilbert 20/1 #######//
				// if( game_file_array.menu(2) == 1)
				if( game_file_array.menu(3) == 1)
				// ##### begin Gilbert 20/1 #######//
				{
					battle.run_loaded();
					// ###### begin Gilbert 22/3 ######//
					deinit_all();
					// ###### end Gilbert 22/3 ######//
				}
				{
					char signalExitFlagBackup = sys.signal_exit_flag;
					sys.signal_exit_flag = 2;
					game.deinit();   // game.deinit() is needed if game_file_array.menu fails
					sys.signal_exit_flag = signalExitFlagBackup;
				}
				break;
			}

			// cancel
			else if( mouse.single_click(BUTTON8_X1, BUTTON8_Y1, BUTTON8_X2, BUTTON8_Y2) )
			{
				break;
			}

		}	// end while
	}	// end scope of vgaLock
}
//---------- End of function Game::scenario_editor_menu ----------//
#endif

//#ifndef DISABLE_MULTI_PLAYER
//---------- Begin of function Game::multi_player_menu ----------//
//
void Game::multi_player_menu(char *cmdLine)
{
	mouse_cursor.set_icon(CURSOR_NORMAL);

	if( cmdLine != NULL && mp_obj.init_flag && mp_obj.is_lobbied() )
	{
		// find player profile
		char* lobbiedName = mp_obj.get_lobbied_name();
		// find profile with the same login name

		if( lobbiedName && player_profile.load_by_login_name(lobbiedName) )
		{
			if( player_profile.mp_new_game_flag == 1 )		// new game
			{
				player_profile.mp_new_game_flag = 0;
				player_profile.save();

				game_file_array.init( player_profile.save_game_path(NULL), "*.SVM" );	// necessary to set the path and extension
				multi_player_game(cmdLine);

				return;
			}
			else if ( player_profile.mp_new_game_flag == 2 )		// load game
			{
				player_profile.mp_new_game_flag = 0;

				// if load file is specified in the profile
				game_file_array.init( player_profile.save_game_path(NULL), "*.SVM" );	// necessary to set the path and extension

				char loadFileName[16];
				if( player_profile.mp_load_file_name[0] )
				{
					strcpy(loadFileName, player_profile.mp_load_file_name);
					strcat( loadFileName, ".SVM" ),		// append extension
					player_profile.mp_load_file_name[0] = '\0';
				}
				else
				{
					loadFileName[0] = '\0';
				}

				player_profile.save();

				if( loadFileName[0]
					&& game_file.load_game(player_profile.save_game_path(NULL), loadFileName) )
				{
					load_mp_game( loadFileName, cmdLine );
				}
				else
				{
					// conventional choose load game
					int loadedRecno;
					if( game_file_array.menu(3, &loadedRecno) == 1 )
					{
						err_when( !loadedRecno );
						load_mp_game(game_file_array[loadedRecno]->file_name, cmdLine);
					}
				}
				{
					char signalExitFlagBackup = sys.signal_exit_flag;
					sys.signal_exit_flag = 2;
					game.deinit();		// game.deinit() is needed if game_file_array.menu fails
					sys.signal_exit_flag = signalExitFlagBackup;
				}
				return;
			}
			else
			{
				return;	// quit

				// player_profile.mp_new_game_flag not specified
				// continue to interface
			}
		}	// end if load by login name
#ifdef DEBUG
		else
		{
			if( lobbiedName )
				err.msg( "Cannot find profile" );
		}
#endif
	}	// end if launchMode

//	char optionFlag[5] = { 1, 1, 1, 1, 1, };

	int refreshFlag = SPOPTION_ALL;
	bool launchMode = (cmdLine != NULL);

	{
		VgaFrontLock vgaLock;

		while(1)
		{
			MSG msg;
			if (PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE))
			{
				if (!GetMessage( &msg, NULL, 0, 0))
				{
					sys.signal_exit_flag = 1;
					// BUGHERE : vga_front is unlocked
					return;
				}
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				continue;
			}
			else if( sys.paused_flag || !sys.active_flag )
			{
				WaitMessage();
				continue;
			}
			if( sys.need_redraw_flag )
			{
				refreshFlag = SPOPTION_ALL;
				sys.need_redraw_flag = 0;
			}

			VgaFrontReLock vgaReLock;

			// -------- display ----------//

			if( refreshFlag )
			{
				if( refreshFlag & SPOPTION_PAGE )
				{
					vga.use_back();
					vga_util.disp_image_file("M_MAIN");

					// ------ display button ------//

					if( player_profile.is_registered() )
					{
						font_thin_black.center_put_paragraph(
							BUTTON2_X1, BUTTON2_Y1, BUTTON2_X2, BUTTON2_Y2, 
							text_game_menu.str_new_game(), 0 );
						font_thin_black.center_put_paragraph(
							BUTTON4_X1, BUTTON4_Y1, BUTTON4_X2, BUTTON4_Y2, 
							text_game_menu.str_load_game(), 0 );
					}

					if( launchMode )
					{
						font_thin_black.center_put_paragraph(
							BUTTON6_X1, BUTTON6_Y1, BUTTON6_X2, BUTTON6_Y2, 
							text_game_menu.str_player_register(), 0);
					}

					// display quit in launch mode,
					// display cancel otherwise
					font_thin_black.center_put_paragraph(
						BUTTON8_X1, BUTTON8_Y1, BUTTON8_X2, BUTTON8_Y2, 
						!launchMode ? text_game_menu.str_cancel() : text_game_menu.str_quit(),
						0 );

					vga_util.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
					vga.use_front();
				}

				refreshFlag = 0;
			}

			sys.blt_virtual_buf();		// blt the virtual front buffer to the screen
			sys.yield();
			mouse.get_event();
			if( config.music_flag )
			{
				if( !music.is_playing(3) )
					music.play(3, sys.cdrom_drive ? MUSIC_CD_THEN_WAV : 0 );
			}
			else
			{
				music.stop();
			}

			// --------- detect -------//

			// detect player register

			if( launchMode
				&& mouse.single_click( BUTTON6_X1, BUTTON6_Y1, BUTTON6_X2, BUTTON6_Y2) )
			{
				player_profile.register_menu();
				if( player_profile.is_registered() )
				{
					refreshFlag = SPOPTION_ALL;		// to display new/load game button
				}
			}

			// detect new game
			else if( player_profile.is_registered()
				&& mouse.single_click( BUTTON2_X1, BUTTON2_Y1, BUTTON2_X2, BUTTON2_Y2) )
			{
				game_file_array.init( player_profile.save_game_path(NULL), "*.SVM" );	// necessary to set the path and extension

				multi_player_game(cmdLine);

				if (launchMode && !sys.signal_exit_flag )
				{
					// if exit from main game, signal_exit_flag is non-zero
					// signal_exit_flag is zero usually means exit from multiplayer setup menu
					refreshFlag = SPOPTION_ALL;
					continue;
				}
				break;
			}

			// detect load game
			else if( player_profile.is_registered()
				&& mouse.single_click( BUTTON4_X1, BUTTON4_Y1, BUTTON4_X2, BUTTON4_Y2) )
			{
				int loadedRecno = 0;

				game_file_array.init( player_profile.save_game_path(NULL), "*.SVM" );	// necessary to set the path and extension

				// ##### begin Gilbert 20/1 #######//
				// if( game_file_array.menu(2, &loadedRecno) == 1 )
				if( game_file_array.menu(3, &loadedRecno) == 1 )
				// ##### begin Gilbert 20/1 #######//
				{
					err_when( !loadedRecno );
					load_mp_game(game_file_array[loadedRecno]->file_name, cmdLine);
				}
				{
					char signalExitFlagBackup = sys.signal_exit_flag;
					sys.signal_exit_flag = 2;
					game.deinit();		// game.deinit() is needed if game_file_array.menu fails
					sys.signal_exit_flag = signalExitFlagBackup;

					if (launchMode && !sys.signal_exit_flag )
					{
						// if exit from main game, signal_exit_flag is non-zero
						// signal_exit_flag is zero usually means exit from multiplayer setup menu
						refreshFlag = SPOPTION_ALL;
						continue;
					}
				}
				break;
			}
			// cancel
			else if( mouse.single_click(BUTTON8_X1, BUTTON8_Y1, BUTTON8_X2, BUTTON8_Y2) )
			{
				break;
			}

		}	// end while
	}	// end scope of vgaLock
}
//#endif


void Game::set_load_game_in_main_menu( char *fileName )
{
	int l = strlen( fileName );
	if( l < MAX_PATH )
		l = MAX_PATH;

	auto_load_file_name = mem_resize( auto_load_file_name, l+1);
	strcpy( auto_load_file_name, fileName );
}


void Game::clear_load_game_in_main_menu()
{
	mem_del( auto_load_file_name );
	auto_load_file_name = NULL;
}

