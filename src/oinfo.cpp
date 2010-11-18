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

//Filename    : OINFO.CPP
//Description : Info class

#include <oinfo.h>
#include <ovga.h>
#include <omodeid.h>
#include <oimgres.h>
#include <ospy.h>
#include <ostr.h>
#include <obutton.h>
#include <omouse.h>
#include <ofirm.h>
#include <oseditor.h>
#include <onews.h>
#include <onation.h> 

#include <ofont.h>
#include <odate.h>
#include <otown.h>
#include <ogame.h>
#include <ogfile.h>
#include <opower.h>
#include <osite.h>
#include <owallres.h>
#include <osys.h>
#include <ounit.h>
#include <ocoltbl.h>
#include <ooptmenu.h>
#include <obitmap.h>
#include <obitmapw.h>
#include <ohelp.h>
#include <time.h>
#include <omodeid.h>
#include <ogame.h>
#include <ocampgn.h>
#include <ot_unit.h>
#include <ot_reps.h>
#include <ot_gmenu.h>
#include <otutor.h>
#include <ofiletxt.h>

#define FIRST_REMAP_KEY       0xE0        // the source color code of the colors to be remapped
														// if change, remember also in ogame.cpp 

//------------ Define static vars ----------------//

//static char* skill_name_array[] =
//{
//	"Combat Skill",
//	"Construction Skill",
//	"Production Skill"
//};

//------ define static vars --------//

static	short *save_buf_1 = NULL;
static	short *save_buf_1b = NULL;
static	short *save_buf_2 = NULL;
static	short *save_buf_2b = NULL;
static	short *save_buf_3 = NULL;
static	short *save_buf_3b = NULL;
static	short *save_buf_4 = NULL;
static	short *save_buf_4b = NULL;

//---- define the customized color code for overriding the STD.COL ----//
//---- such table also exists in ogame.cpp ----------------------------//
static short remap_location_array[] =
{
     0xBC, 0xDC,   // the first remap table is for independent units
     0xA0, 0xC0,   // following are eight remap table for each color code
     0xA4, 0xC4,
     0xA8, 0xC8,
     0xAC, 0xCC,
     0xB0, 0xD0,
	  0xB4, 0xD4,
     0xB8, 0xD8,
     0xBC, 0xDC,
};
static	short	colour_buffer[8];

static ResourceIdx res_hot_key;

	//-------- initialize color remap table -------//


//-------- Begin of function Info::Info --------//
//
Info::Info() : report_array(sizeof(short), 50),
					report_array2(sizeof(short), 50),
					talk_msg_disp_array(sizeof(TalkMsgDisp), 50)
{
	info_background_bitmap = NULL;
	heading_background_bitmap = NULL;
	heading_background_bitmap2= NULL;

	game_speed_info_display_delay = 0;
	game_pause_info_display_delay = 0;
	
//	spy_execute = 0;
}
//--------- End of function Info::Info ---------//


//-------- Begin of function Info::~Info --------//
//
Info::~Info()
{
	deinit();

	err_when( save_buf_1 );
}
//--------- End of function Info::~Info ---------//


//-------- Begin of function Info::init --------//
//
void Info::init(int yyyy, int mm, int dd)
{
	deinit();

	game_day   = dd;
	game_month = mm;
	game_year  = yyyy;

	game_start_date = date.julian( game_year, game_month, game_day );

	game_date  = game_start_date;

	year_day   = date.day_year( game_year, game_month, game_day );

	year_passed= 0;      // no. of years has been passed since the game begins

	goal_deadline = date.julian( date.year(game_start_date)+config.goal_year_limit,
						 date.month(game_start_date),
						 date.day(game_start_date) );

	goal_difficulty  = 0;
	goal_score_bonus = 0;

	//------ reset report browsers recno -----//

	browse_nation_recno  = 0;
	browse_race_recno    = 0;
	browse_firm_recno    = 0;
	browse_income_recno	= 0;
	browse_expense_recno	= 0;
	browse_troop_recno   = 0;
	browse_unit_recno    = 0;
	browse_tech_recno    = 0;
	browse_god_recno		= 0;
	browse_town_recno    = 0;
	browse_spy_recno     = 0;
	browse_caravan_recno = 0;
	browse_ship_recno	   = 0;
	browse_talk_msg_recno= 0;
	browse_news_recno	   = 0;
	browse_ai_action_recno = 0;
	browse_ai_attack_recno = 0;

	//------ vars of the nation report ------//

	nation_report_mode = NATION_REPORT_TALK;
	player_reply_mode  = 0;
	chat_receiver_type = CHAT_RECEIVER_CURRENT;
	// ####### begin Gilbert 28/6 ########//
	memset( remote_chat_array, 0, sizeof(remote_chat_array) );
	// ####### end Gilbert 28/6 ########//

	//----------------------------------//

	start_play_time = m.get_time();	// the time player start playing the game
	total_play_time = 0;             // total time the player has played in all saved games
	display_campaign_mission_briefing = 0;
	display_hot_key = 0;

	// these will be updated during loading game and saving game

	err_when( MAX_REMOTE_CHAT_STR < DISP_NEWS_COUNT );			// it must not fewer then the maximum number of news that can be displayed on the screen at a time.
}
//--------- End of function Info::init ---------//


//-------- Begin of function Info::deinit --------//
//
void Info::deinit()
{
	if( info_background_bitmap )
	{
		mem_del( info_background_bitmap );
		info_background_bitmap = NULL;
	}
	if( heading_background_bitmap )
	{
		mem_del( heading_background_bitmap );
		heading_background_bitmap = NULL;
	}
	if( heading_background_bitmap2 )
	{
		mem_del( heading_background_bitmap2 );
		heading_background_bitmap2 = NULL;
	}
}
//--------- End of function Info::deinit ---------//


//------- Begin of function Info::init_random_seed ------//
//
// [unsigned] randomSeed - if given, it will be the random seed of the game.
//									random seed. otherwise a random seed will be
//								   picked.
//								   (default:0)
//
void Info::init_random_seed(int randomSeed)
{
	if( randomSeed )
		random_seed = randomSeed;
	else
	{
		randomSeed = time(NULL);
		randomSeed = (int) _rotr( randomSeed, 4 );
		if( randomSeed < 0 )
			randomSeed = ~randomSeed;
		if( randomSeed == 0 )
			randomSeed = 1;
		random_seed = randomSeed;
	}

	m.set_random_seed(random_seed);

	//------ write random seed --------//

	if( sys.testing_session )
	{
		File fileMapSeed;

		fileMapSeed.file_create( "MAP.RS" );

		String str = m.format(random_seed,1);

		fileMapSeed.file_write(str, str.len());
	}
}
//------- End of function Info::init_random_seed ------//


//-------- Begin of function Info::next_day ---------//
//
void Info::next_day()
{
	if( ++game_day > 30 )
		game_day = 30;            // game_day is limited to 1-30 for
										  // calculation of e.g. revenue_30days()
	game_date++;

	week_day=game_date%7;

	//-----------------------------------------//

	if( date.month(game_date) != game_month )
	{
		game_day   = 1;
		game_month = date.month(game_date);

		town_array.next_month();
		firm_array.next_month();
		nation_array.next_month();
	}

	if( date.year(game_date) != game_year )
	{
		game_month = 1;
		game_year  = date.year(game_date);
		year_passed++;

		firm_array.next_year();
		nation_array.next_year();
	}

	//-------- set year_day ----------//

	year_day = date.day_year( game_year, game_month, game_day );

	//--- if a spy is viewing secret reports of other nations ---//

	if( viewing_spy_recno )
		process_viewing_spy();

	//-------- deadline approaching message -------//

	if( !game.game_has_ended && game.win_deadline )
	{
		int dayLeft = game.win_deadline-game_date;
		int yearLeft = dayLeft/365;

		if( dayLeft%365==0 && yearLeft>=1 && yearLeft<=5 )
			news_array.goal_deadline( yearLeft, 0 );

		if( dayLeft==0 )		// deadline arrives, everybody loses the game
			game.game_end(0, 0);
	}
}
//---------- End of function Info::next_day --------//


//-------- Begin of function Info::disp_panel --------//
//
void Info::disp_panel()
{
	if (current_display_mode.mode_id == MODE_ID_800x600x16)
	{
		image_interface.put_to_buf( &vga_back, "MAINSCR" );
		if( config.race_id < 0 )
			vga_back.put_bitmap(313, 0, image_interface.read("800-HUM"));
	}
	else if (current_display_mode.mode_id == MODE_ID_1024x768x16)
	{
		image_interface.put_to_buf( &vga_back, "MSCR1024" );
		if( config.race_id < 0 )
			vga_back.put_bitmap(800, 1, image_interface.read("1024-HUM"));
	}
	else
		err_here();
	
	//------ keep a copy of bitmap of the panel texture -----//

	if( !info_background_bitmap ) 
		info_background_bitmap = (short *)mem_add( BitmapW::size(INFO_X2-INFO_X1+1, INFO_Y2-INFO_Y1+1));
	vga_back.read_bitmapW( INFO_X1, INFO_Y1, INFO_X2, INFO_Y2, info_background_bitmap );

	if( !heading_background_bitmap )
		heading_background_bitmap = (short *)mem_add( BitmapW::size(TOP_MENU_X2-TOP_MENU_X1+1, TOP_MENU_Y2-TOP_MENU_Y1+1) );
	vga_back.read_bitmapW( TOP_MENU_X1, TOP_MENU_Y1, TOP_MENU_X2, TOP_MENU_Y2, heading_background_bitmap );

//	if (current_display_mode.mode_id == MODE_ID_1024x768x16)
//	{
//		if( !heading_background_bitmap2 )
//			heading_background_bitmap2 = (short *)mem_add( BitmapW::size(31, 101) );
//		vga_back.read_bitmapW( REPU_BUTTON_X1, REPU_BUTTON_Y1, REPU_BUTTON_X1 +100, REPU_BUTTON_Y1 +30, heading_background_bitmap2 );
//	}
}
//--------- End of function Info::disp_panel ---------//


//-------- Begin of function Info::disp --------//
//
// Display the side info area.
//
void Info::disp()
{
//	mouse.handle_flicking = 0;		// since it will be called by Firm detect functions directly which may have set mouse.handle_flicking to 1 first, so we need to cancel it here

	if( !power.enable_flag )
		return;

	if( sys.signal_exit_flag )
		return;

	if( option_menu.is_active() )
		return;
	
#ifdef USE_FLIP
	vga.use_back();
#endif

	// ######## begin Gilbert 23/12 #######//
	help.hide_area(INFO_X1, INFO_Y1, INFO_X2, INFO_Y2);
	// ######## end Gilbert 23/12 #######//

	vga.active_buf->put_bitmapW( INFO_X1, INFO_Y1, info_background_bitmap );

//	char *nationPict = image_gameif.get_ptr("REMSCR");
//	vga.active_buf->put_bitmap_trans_remap(INFO_X1+5, INFO_Y1-255, nationPict, game.get_color_remap_table(nation_array.player_recno, 0) );

	vga.active_buf->put_bitmap_trans( INFO_X1+3, INFO_Y1-125, image_gameif.read("REMSCR") );

//	if (spy_execute)
//	{
//		vga.active_buf->put_bitmap( INFO_X1+17, INFO_Y1+96, image_gameif.read("EXECUTE") );
//		font_whbl.center_put( INFO_X1 +11, INFO_Y1 +100, INFO_X1 +219, INFO_Y1 +130, "You have just executed a spy");
//	}
		
#ifdef USE_FLIP
	vga.use_front();
#endif

	//------- use front buffer -------//

	int saveUseBackBuf = vga.use_back_buf;

	vga.use_front();
	//------ if units/firm selected, display info --------//

	// ##### begin Gilbert 1/2 ######//
//	if( scenario_editor.is_enable() && scenario_editor.edit_mode != SEDIT_MODE_NONE)
//	{
//		scenario_editor.disp(INFO_REPAINT);
//	}
//	else
	if( firm_array.selected_recno )
	// ##### end Gilbert 1/2 ######//
	{
		firm_array[firm_array.selected_recno]->disp_info_both(INFO_REPAINT);
//		spy_execute = 0;
	}
	else if( town_array.selected_recno )
	{
		town_array[town_array.selected_recno]->disp_info(INFO_REPAINT);
//		spy_execute = 0;
	}
	else if( site_array.selected_recno )
	{
		site_array[site_array.selected_recno]->disp_info(INFO_REPAINT);
//		spy_execute = 0;
	}
	else if( unit_array.selected_recno )
	{
		unit_array[unit_array.selected_recno]->disp_info(INFO_REPAINT);
//		spy_execute = 0;
	}
	else if( wall_res.selected_x_loc >= 0 )
	{
		wall_res.disp_info(INFO_REPAINT);
//		spy_execute = 0;
	}

	// ####### begin Gilbert 23/2 ######//
	// if in scenario editor mode, call repaint to both normal interface and editor interface
	// such that normal interface is called with INFO REPAINT when changing selected object in editor's mode
	if( scenario_editor.is_enable())
	{
		scenario_editor.disp(INFO_REPAINT);
	}
	// ####### end Gilbert 23/2 ########//

#ifdef USE_FLIP
	vga.blt_buf(INFO_X1, INFO_Y1-125, INFO_X2, INFO_Y2, 0);
#endif

	// ######## begin Gilbert 23/12 #######//
	help.show_area();
	// ######## end Gilbert 23/12 #######//

	//----- restore use back buffer if it was ----//

	if( saveUseBackBuf )
		vga.use_back();
}
//-------- End of function Info::disp --------//


//-------- Begin of function Info::update --------//

void Info::update()
{
	if( !power.enable_flag )
		return;

	if( option_menu.is_active() )
		return;

	//-------------------------------------------//

	disp_heading();

	//------- use front buffer -------//

	int saveUseBackBuf = vga.use_back_buf;

	vga.use_front();

#ifdef USE_FLIP
	vga.use_back();
#endif

	vga.active_buf->put_bitmap_trans( INFO_X1+3, INFO_Y1-125, image_gameif.read("REMSCR") );

#ifdef USE_FLIP
	vga.use_front();
#endif

	//-------------------------------------------//

	// ##### begin Gilbert 13/10 ######//
	if( scenario_editor.is_enable() && scenario_editor.edit_mode != SEDIT_MODE_NONE)
	{
		scenario_editor.disp(INFO_UPDATE);
	}
	else if( firm_array.selected_recno )
	// ##### end Gilbert 13/10 ######//
	{
		firm_array[firm_array.selected_recno]->disp_info_both(INFO_UPDATE);
#ifdef DEBUG
		if( town_array.selected_recno || site_array.selected_recno || unit_array.selected_recno )
		{
			err.msg( "invalid selected_recno" );
		}
#endif
	}
	else if( town_array.selected_recno )
	{
		town_array[town_array.selected_recno]->disp_info(INFO_UPDATE);
#ifdef DEBUG
		if( firm_array.selected_recno || site_array.selected_recno || unit_array.selected_recno )
		{
			err.msg( "invalid selected_recno" );
		}
#endif
	}
	else if( site_array.selected_recno )
	{
		site_array[site_array.selected_recno]->disp_info(INFO_UPDATE);
#ifdef DEBUG
		if( firm_array.selected_recno || town_array.selected_recno || unit_array.selected_recno )
		{
			err.msg( "invalid selected_recno" );
		}
#endif
	}
	else if( unit_array.selected_recno )
	{
		unit_array[unit_array.selected_recno]->disp_info(INFO_UPDATE);
#ifdef DEBUG
		if( firm_array.selected_recno || town_array.selected_recno || site_array.selected_recno )
		{
			err.msg( "invalid selected_recno" );
		}
#endif
	}
	else if( wall_res.selected_x_loc >= 0 )
	{
		wall_res.disp_info(INFO_UPDATE);
	}
	// ######## begin Gilbert 8/3 #######//
	else
	{
		vga.active_buf->put_bitmapW( INFO_X1, INFO_Y1, info_background_bitmap );
	}
	// ######## end Gilbert 8/3 #######//

	//----- restore use back buffer if it was ----//

	if( saveUseBackBuf )
		vga.use_back();
}
//-------- End of function Info::update --------//

//-------- Begin of function Info::disp_heading --------//

void Info::disp_heading()
{
#ifdef USE_FLIP
	vga_back.put_bitmapW(TOP_MENU_X1, TOP_MENU_Y1, heading_background_bitmap);
//	if (current_display_mode.mode_id == MODE_ID_1024x768x16)
//		vga_back.put_bitmapW(REPU_BUTTON_X1, REPU_BUTTON_Y1, heading_background_bitmap2);
	vga.use_back();
#endif

	//---- display info on the top menu area ----//

	if (game_speed_info_display_delay > 0 && config.frame_speed > 0)
	{
		String str;
		int i;
		// str = "”Œœ∑ÀŸ∂» ";
		str = text_game_menu.str_game_speed();
		str += " ";
		if (config.frame_speed == 99)
			i = 9;
		else
			i = config.frame_speed /3;
		str += m.format(i, 4);
		font_news.center_put( ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2 -(font_news.height()<<1), str);

		game_speed_info_display_delay--;
	}
	
	if (config.frame_speed == 0)
		image_spict.put_back( ((ZOOM_X1 + ZOOM_X2)>>1) -109,
							((ZOOM_Y1 + ZOOM_Y2)>>1) +10, "GPAUSE", 1 );
	else
	if (game_pause_info_display_delay > 0 && config.frame_speed > 0)
	{
		image_spict.put_back( ((ZOOM_X1 + ZOOM_X2)>>1) -109,
							((ZOOM_Y1 + ZOOM_Y2)>>1) +10, "GRESUME", 1 );
		game_pause_info_display_delay --;
	}

	if (display_campaign_mission_briefing == 3 && !option_menu.is_active())
	{
		game.campaign()->disp_in_game_msg( game.campaign()->intro_text() );

		//------- display plot ---------//
		if( game.campaign()->plot_id )
		{
			char *plotTextPtr = game.campaign()->plot_text();
			if( plotTextPtr )
			{
			//	vga.flip();
				game.campaign()->disp_in_game_msg( plotTextPtr );
			}
		}
		display_campaign_mission_briefing = 0;
	}
	// delay one frame for display mission briefing 
	// in order to clear the in-game menu
	if (display_campaign_mission_briefing > 0)
		display_campaign_mission_briefing ++;
	
	if (display_hot_key > 2 && !option_menu.is_active())
	{
		String str;
		str  = DIR_RES;
		str += "HOTKEY.RES";
		res_hot_key.init(str, 0);
		int   dataSize;
		char	*hot_key_buf = NULL;
		int	hot_key_buf_size = 0;
		File* filePtr;

		if (display_hot_key == 3)
			filePtr = res_hot_key.get_file("HOTKEY1", dataSize);
		else
			filePtr = res_hot_key.get_file("HOTKEY2", dataSize);

		if( !filePtr )       // if error getting the tutor resource
			err_here();

		//------ Open the file and allocate buffer -----//
		FileTxt fileTxt( filePtr, dataSize );  // initialize fileTxt with an existing file stream
		if( dataSize > hot_key_buf_size )
		{
			hot_key_buf      = mem_resize( hot_key_buf, dataSize );       // allocate a buffer larger than we need for the largest size possible
			hot_key_buf_size = dataSize;
		}
		char	*textPtr = hot_key_buf;
		fileTxt.read_paragraph(textPtr, hot_key_buf_size);
	//	font_mid.center_put_paragraph( 0, 0, VGA_WIDTH, VGA_HEIGHT, textPtr, 2, 0, 0 );
		Campaign::put_center_text(ZOOM_X1, ZOOM_Y1, textPtr, 0, &font_whbl);

		if( hot_key_buf )
		{
			mem_del(hot_key_buf);
			hot_key_buf = NULL;
		}


		hot_key_buf = NULL;
		hot_key_buf_size = 0;
		if (display_hot_key == 3)
			filePtr = res_hot_key.get_file("HOTKEY1A", dataSize);
		else
			filePtr = res_hot_key.get_file("HOTKEY2A", dataSize);

		if( !filePtr )       // if error getting the tutor resource
			err_here();

		//------ Open the file and allocate buffer -----//
		FileTxt fileTxt2( filePtr, dataSize );  // initialize fileTxt2 with an existing file stream
		if( dataSize > hot_key_buf_size )
		{
			hot_key_buf      = mem_resize( hot_key_buf, dataSize );       // allocate a buffer larger than we need for the largest size possible
			hot_key_buf_size = dataSize;
		}
		textPtr = hot_key_buf;
		fileTxt2.read_paragraph(textPtr, hot_key_buf_size);
	//	font_mid.center_put_paragraph( 0, 0, VGA_WIDTH, VGA_HEIGHT, textPtr, 2, 0, 0 );
		Campaign::put_center_text(107 +ZOOM_X1, ZOOM_Y1, textPtr, 0, &font_whbl, 0);

		if( hot_key_buf )
		{
			mem_del(hot_key_buf);
			hot_key_buf = NULL;
		}
	}
	// delay one frame for display hot key
	// in order to clear the in-game menu
	if (display_hot_key > 0 && display_hot_key < 3)
		display_hot_key ++;
	
	//---------- display date -----------//

	font_snds.use_max_height();
	font_snds.disp( DATE_X1, DATE_Y1, date.date_str(game_date,1), DATE_X1 +DATE_LENGTH, 1);
	font_snds.use_std_height();

	if( !nation_array.player_recno )		// the player has lost the game
	{
		
		if (current_display_mode.mode_id == MODE_ID_800x600x16)
		{
			font_snds.disp( FOOD_X1, FOOD_Y1, "", FOOD_X1 +FOOD_LENGTH);	// clear the display 
			font_snds.disp( CASH_X1, CASH_Y1, "", CASH_X1 +CASH_LENGTH);	
		}
		else if (current_display_mode.mode_id == MODE_ID_1024x768x16)
		{
			font_snds.disp( FOOD_X1, FOOD_Y1, "", FOOD_X1 +FOOD_LENGTH);	// clear the display 
			font_snds.disp( CASH_X1, CASH_Y1, "", CASH_X1 +CASH_LENGTH);	
			font_snds.disp( FOOD_X1-7, FOOD_Y1+12, "", FOOD_X1 +FOOD_LENGTH);
			font_snds.disp( CASH_X1-7, CASH_Y1+12, "", CASH_X1 +CASH_LENGTH);	
		}
		else
			err_here();
		
		char *thumbBmp;
		if( config.race_id > 0 )
		{
			if (current_display_mode.mode_id == MODE_ID_800x600x16)
				thumbBmp = image_icon.get_ptr( "REPU_DW" );
			else if (current_display_mode.mode_id == MODE_ID_1024x768x16)
				thumbBmp = image_icon.get_ptr( "REDW1024" );
			else
				err_here();
			vga.active_buf->put_bitmap(REPU_BUTTON_X1, REPU_BUTTON_Y1, thumbBmp);
		}
		if (current_display_mode.mode_id == MODE_ID_800x600x16)
			font_snds.disp( REPU_BUTTON_X1 +30, REPU_BUTTON_Y1 +12, "", REPU_BUTTON_X1 +100);
		else if (current_display_mode.mode_id == MODE_ID_1024x768x16)
		{
			if( config.race_id > 0 )
			{
				font_snds.disp( REPU_BUTTON_X1+12, REPU_BUTTON_Y1 +27, "", REPU_BUTTON_X1 +50);
				font_snds.disp( REPU_BUTTON_X1+2, REPU_BUTTON_Y1 +39, "", REPU_BUTTON_X1 +50);
			}
			else
			{
				font_snds.disp( REPU_BUTTON_X1+5, REPU_BUTTON_Y1 +27, "", REPU_BUTTON_X1 +50);
				font_snds.disp( REPU_BUTTON_X1-2, REPU_BUTTON_Y1 +39, "", REPU_BUTTON_X1 +50);
			}
		}
		else
			err_here();
	}
	else
	{
		//-------------------------------------------------//

		String  str;
		Nation* nationPtr;

		if( config.show_ai_info )
			nationPtr = nation_array[ info.default_viewing_nation_recno ];
		else
			nationPtr = ~nation_array;

		//------- display food and net food change --------//

		char* strPtr; 
		if (current_display_mode.mode_id == MODE_ID_800x600x16)
		{
		//	strPtr = nationPtr->food_str();
		//	font_snds.disp( FOOD_X1, FOOD_Y1, strPtr, FOOD_X1 +FOOD_LENGTH);
			int tempx; 
			strPtr = nationPtr->total_food_str();
			if (strPtr[0] == '-')
				tempx = font_red.disp( FOOD_X1, FOOD_Y1, strPtr, FOOD_X1 +FOOD_LENGTH);
			else
				tempx = font_snds.disp( FOOD_X1, FOOD_Y1, strPtr, FOOD_X1 +FOOD_LENGTH);

			strPtr = nationPtr->annual_food_str();
			if (strPtr[2] == '-')
				font_red.disp( tempx, FOOD_Y1, strPtr, FOOD_X1 +FOOD_LENGTH);
			else
				font_snds.disp( tempx, FOOD_Y1, strPtr, FOOD_X1 +FOOD_LENGTH);
		}
		else if (current_display_mode.mode_id == MODE_ID_1024x768x16)
		{
			strPtr = nationPtr->total_food_str();
			if (strPtr[0] == '-')
				font_red.disp( FOOD_X1, FOOD_Y1, strPtr, FOOD_X1 +FOOD_LENGTH);
			else
				font_snds.disp( FOOD_X1, FOOD_Y1, strPtr, FOOD_X1 +FOOD_LENGTH);

			strPtr = nationPtr->annual_food_str();
			if (strPtr[2] == '-')
				font_red.disp( FOOD_X1-7, FOOD_Y1 +12, strPtr, FOOD_X1 +FOOD_LENGTH);
			else
				font_snds.disp( FOOD_X1-7, FOOD_Y1 +12, strPtr, FOOD_X1 +FOOD_LENGTH);
		}
		else
			err_here();
			
		//------- display cash and profit --------//
		if (current_display_mode.mode_id == MODE_ID_800x600x16)
		{
		//	strPtr = nationPtr->cash_str();
		//	font_snds.disp( CASH_X1, CASH_Y1, strPtr, CASH_X1 +CASH_LENGTH);

			int tempx;
			if (nationPtr->increased_cash > 0)
			{
				int tempCash = (int)(nationPtr->cash - nationPtr->increased_cash);
				if( tempCash >= 0 )
				{
					strPtr = m.format( (int)tempCash, 4 );			// format type 4 - no thousand separators
				}
				else
				{
					str = "-";
					str += m.format( (int)-tempCash, 4 );		// format type 4 - no thousand separators
					strPtr = str;
				}
				if (nationPtr->increased_cash > 1000)
					nationPtr->increased_cash = nationPtr->increased_cash - 1000;
				else
				if (nationPtr->increased_cash > 100)
					nationPtr->increased_cash = nationPtr->increased_cash - 100;
				else
				if (nationPtr->increased_cash > 10)
					nationPtr->increased_cash = nationPtr->increased_cash - 10;
				else
					nationPtr->increased_cash = nationPtr->increased_cash - 1;
							
				if (nationPtr->increased_cash < 0)
					nationPtr->increased_cash = 0;
			}
			else
				strPtr = nationPtr->total_cash_str();

			if (strPtr[0] == '-')
				tempx = font_red.disp( CASH_X1, CASH_Y1, strPtr, CASH_X1 +CASH_LENGTH);
			else
				tempx = font_snds.disp( CASH_X1, CASH_Y1, strPtr, CASH_X1 +CASH_LENGTH);

			strPtr = nationPtr->annual_cash_str();
			if (strPtr[2] == '-')
				font_red.disp( tempx, CASH_Y1, strPtr, CASH_X1 +CASH_LENGTH);
			else
				font_snds.disp( tempx, CASH_Y1, strPtr, CASH_X1 +CASH_LENGTH);
		}
		else if (current_display_mode.mode_id == MODE_ID_1024x768x16)
		{
			if (nationPtr->increased_cash > 0)
			{
				int tempCash = (int)(nationPtr->cash - nationPtr->increased_cash);
				if( tempCash >= 0 )
				{
					strPtr = m.format( (int)tempCash, 4 );			// format type 4 - no thousand separators
				}
				else
				{
					str = "-";
					str += m.format( (int)-tempCash, 4 );		// format type 4 - no thousand separators
					strPtr = str;
				}
				if (nationPtr->increased_cash > 1000)
					nationPtr->increased_cash = nationPtr->increased_cash - 1000;
				else
				if (nationPtr->increased_cash > 100)
					nationPtr->increased_cash = nationPtr->increased_cash - 100;
				else
				if (nationPtr->increased_cash > 10)
					nationPtr->increased_cash = nationPtr->increased_cash - 10;
				else
					nationPtr->increased_cash = nationPtr->increased_cash - 1;
							
				if (nationPtr->increased_cash < 0)
					nationPtr->increased_cash = 0;
			}
			else
				strPtr = nationPtr->total_cash_str();

			if (strPtr[0] == '-')
				font_red.disp( CASH_X1, CASH_Y1, strPtr, CASH_X1 +CASH_LENGTH);	
			else
				font_snds.disp( CASH_X1, CASH_Y1, strPtr, CASH_X1 +CASH_LENGTH);	

			strPtr = nationPtr->annual_cash_str();
			if (strPtr[2] == '-')
				font_red.disp( CASH_X1-7, CASH_Y1+12, strPtr, CASH_X1 +CASH_LENGTH);
			else
				font_snds.disp( CASH_X1-7, CASH_Y1+12, strPtr, CASH_X1 +CASH_LENGTH);
		}
		else
			err_here();

		//------- display reputation ---------//

		int   dispValue, dispChange, tempx;
		float changeAmt;

		if( config.race_id > 0 )
		{
			dispValue  = (int) nationPtr->reputation;
			changeAmt  = nationPtr->reputation_change_365days();
		}
		else
		{
			dispValue  = (int) nationPtr->live_points;
			changeAmt  = nationPtr->live_points_change_365days();
		}

		dispChange = (int) changeAmt;

		char *thumbBmp;
		if( config.race_id > 0 )
		{
			if (current_display_mode.mode_id == MODE_ID_800x600x16)
				thumbBmp = image_icon.get_ptr(changeAmt >= 0 ? "REPU_UP" : "REPU_DW" );
			else if (current_display_mode.mode_id == MODE_ID_1024x768x16)
				thumbBmp = image_icon.get_ptr(changeAmt >= 0 ? "REUP1024" : "REDW1024" );
			else 
				err_here();
			vga.active_buf->put_bitmap(REPU_BUTTON_X1, REPU_BUTTON_Y1, thumbBmp);
		}

		if( dispValue >= 0 )
		{
			str = m.format( (int)dispValue, 4 );			// format type 4 - no thousand separators
			if (current_display_mode.mode_id == MODE_ID_800x600x16)
			{
				tempx = font_snds.disp( REPU_BUTTON_X1 +30, REPU_BUTTON_Y1 +12, str, REPU_BUTTON_X1 +120);
			}
			else if (current_display_mode.mode_id == MODE_ID_1024x768x16)
			{
				if ( config.race_id > 0 )
					font_snds.disp( REPU_BUTTON_X1 +12, REPU_BUTTON_Y1 +27, str, REPU_BUTTON_X1 +50);
				else
					font_snds.disp( REPU_BUTTON_X1 +5, REPU_BUTTON_Y1 +27, str, REPU_BUTTON_X1 +50);
			}
			else
				err_here();
		}
		else
		{
			str  = "-";
			str += m.format( (int)-dispValue, 4 );		// format type 4 - no thousand separators
			if (current_display_mode.mode_id == MODE_ID_800x600x16)
			{
				tempx = font_red.disp( REPU_BUTTON_X1 +30, REPU_BUTTON_Y1 +12, str, REPU_BUTTON_X1 +120);
			}
			else if (current_display_mode.mode_id == MODE_ID_1024x768x16)
			{
				if ( config.race_id > 0 )
					font_red.disp( REPU_BUTTON_X1 +12, REPU_BUTTON_Y1 +27, str, REPU_BUTTON_X1 +50);
				else
					font_red.disp( REPU_BUTTON_X1 +5, REPU_BUTTON_Y1 +27, str, REPU_BUTTON_X1 +50);
			}
			else
				err_here();
		}
		
		if( dispChange )
		{
			str = " (";
			if( dispChange > 0 )
				str += "+";
			else
				str += "-";

			str += m.format( abs(dispChange), 4);
			str += ")";
			
			if (str[2] == '-')
			{
				if (current_display_mode.mode_id == MODE_ID_800x600x16)
				{
					font_red.disp( tempx, REPU_BUTTON_Y1 +12, str, REPU_BUTTON_X1 +120);
				}
				else if (current_display_mode.mode_id == MODE_ID_1024x768x16)
				{
					if ( config.race_id > 0 )
						font_red.disp( REPU_BUTTON_X1 +2, REPU_BUTTON_Y1 +39, str, REPU_BUTTON_X1 +50);
					else
						font_red.disp( REPU_BUTTON_X1 -2, REPU_BUTTON_Y1 +39, str, REPU_BUTTON_X1 +50);
				}
				else
					err_here();
			}
			else
			{
				if (current_display_mode.mode_id == MODE_ID_800x600x16)
				{
					font_snds.disp( tempx, REPU_BUTTON_Y1 +12, str, REPU_BUTTON_X1 +120);
				}
				else if (current_display_mode.mode_id == MODE_ID_1024x768x16)
				{
					if ( config.race_id > 0 )
						font_snds.disp( REPU_BUTTON_X1 +2, REPU_BUTTON_Y1 +39, str, REPU_BUTTON_X1 +50);
					else
						font_snds.disp( REPU_BUTTON_X1 -2, REPU_BUTTON_Y1 +39, str, REPU_BUTTON_X1 +50);
				}
				else
					err_here();
			}
		}
	}

#ifdef USE_FLIP
	vga.use_front();
#endif
}
//-------- End of function Info::disp_heading --------//

//-------- Begin of function Info::detect --------//

int Info::detect()
{
	//------------ detect objects ------------//

	// ###### begin Gilbert 24/2 #######//
	if( scenario_editor.is_enable() && scenario_editor.edit_mode != SEDIT_MODE_NONE)
	{
		scenario_editor.detect();
	}
	else if( firm_array.selected_recno )
	// ###### end Gilbert 24/20 #######//
	{
		firm_array[firm_array.selected_recno]->detect_info_both();
	}
	else if( town_array.selected_recno )
	{
		town_array[town_array.selected_recno]->detect_info();
	}
	else if( site_array.selected_recno )
	{
		site_array[site_array.selected_recno]->detect_info();
	}
	else if( unit_array.selected_recno )
	{
		unit_array[unit_array.selected_recno]->detect_info();
	}

	return 0;
}
//-------- End of function Info::detect --------//


//-------- Begin of function Info::draw_selected --------//

void Info::draw_selected()
{
	if( firm_array.selected_recno )
		firm_array[firm_array.selected_recno]->draw_selected();

	else if( town_array.selected_recno )
		town_array[town_array.selected_recno]->draw_selected();

	else if( site_array.selected_recno )
		site_array[site_array.selected_recno]->draw_selected();

	else if( wall_res.selected_x_loc >= 0 )
		wall_res.draw_selected();
}
//-------- End of function Info::draw_selected --------//


//-------- Begin of function Info::get_report_data --------//

short Info::get_report_data(int recNo)
{
	err_when( recNo<1 || recNo>report_array.size() );

	return *((short*)report_array.get(recNo));
}
//-------- End of function Info::get_report_data --------//


//-------- Begin of function Info::get_report_data2 --------//

short Info::get_report_data2(int recNo)
{
	err_when( recNo<1 || recNo>report_array2.size() );

	return *((short*)report_array2.get(recNo));
}
//-------- End of function Info::get_report_data2 --------//


//-------- Begin of function Info::process_viewing_spy --------//

void Info::process_viewing_spy()
{
	//---- check if the viewing spy is still valid ----//

	int isValid=1;

	if( spy_array.is_deleted(viewing_spy_recno) )	// the spy is dead
	{
		isValid = 0;
	}
	else
	{
		Spy* spyPtr = spy_array[viewing_spy_recno];

		//-- check if the spy still stay in the same place --//

		if( spyPtr->spy_place_nation_recno() != info.viewing_nation_recno )
		{
			isValid = 0;
		}
		else
		{
		/*
			//--- on average, a spy will get caught in 5 to 15 days when viewing the secret of its enemy ---//

			if( m2.random(5+spyPtr->spy_skill/5) )		// use m2 to avoid multiplayer sync problem
			{
				spyPtr->set_exposed(COMMAND_PLAYER);
				isValid = 0;
			}
		*/
		}
	}

	if( !isValid )		//-- if not valid, set the mode back to normal viewing mode
		sys.set_view_mode(MODE_NORMAL);
}
//-------- End of function Info::process_viewing_spy --------//


//------- Begin of function Info::play_time_str --------//
//
char* Info::play_time_str()
{
	int totalMin = total_play_time / (60*1000);
	int playHour = totalMin / 60;
	int playMin  = totalMin - playHour*60;

//	static String str;
//	str = "";
//	if( playHour > 0 )
//	{
//		str += playHour;
//		str += translate.process( playHour>1 ? " hours" : " hour" );
//		str += translate.process( " and " );
//	}
//	str += playMin;
//	str += translate.process( playMin>1  ? " minutes" : " minute" );
//	return str;

	return text_reports.str_duration_hm( playHour, playMin );
}
//---------- End of function Info::play_time_str ---------//


//------- Begin of function Info::game_duration_str --------//
//
char* Info::game_duration_str()
{
	//------- get the true game start date --------//

	int gameStartDate;

	//-- For scenarios (whose goal_difficulty are > 0 ), the actual game start date is no info.game_start_date, we must calculate it

	if( info.goal_difficulty )
	{
		gameStartDate =  date.julian( date.year(info.goal_deadline)-config.goal_year_limit,
							  date.month(info.goal_deadline),
							  date.day(info.goal_deadline) );
	}
	else
	{
		gameStartDate = info.game_start_date;
	}

	//---------------------------------------------//

	int totalDay = info.game_date - gameStartDate;
	int playYear = totalDay / 365;
	int playDay  = totalDay - playYear*365;

//	static String str;
//	str = "";
//	if( playYear > 0 )
//	{
//		str += playYear;
//		str += translate.process( playYear>1 ? " years" : " year" );
//		str += translate.process( " and " );
//	}
//	str += playDay;
//	str += translate.process( playDay>1  ? " days" : " day" );
// return str;

	return text_reports.str_duration_yd( playYear, playDay );
}
//---------- End of function Info::game_duration_str ---------//


//------- Begin of function Info::save_game_scr --------//
//
void Info::save_game_scr()
{
	err_when( save_buf_1 );
	// top and buttom
	if( 0 < ZOOM_Y1 )
	{
		save_buf_1 = vga_front.save_area(0, 0, VGA_WIDTH-1, ZOOM_Y1-1);
		save_buf_1b = vga_back.save_area(0, 0, VGA_WIDTH-1, ZOOM_Y1-1);		// save the back buffer also as the top area of the back buf is used for font display 
	}
	else
	{
		save_buf_1 = NULL;	// clear it so rest_game_scr won't restore it
		save_buf_1b = NULL;
	}

	if( ZOOM_Y2 < VGA_HEIGHT-1 )
	{
		save_buf_2 = vga_front.save_area(0, ZOOM_Y2+1, VGA_WIDTH-1, VGA_HEIGHT-1);
#ifdef USE_FLIP
		save_buf_2b = vga_back.save_area(0, ZOOM_Y2+1, VGA_WIDTH-1, VGA_HEIGHT-1);
#endif
	}
	else
	{
		save_buf_2 = NULL;
		save_buf_2b = NULL;
	}

	// left and right
	if( 0 < ZOOM_X1 )
	{
		save_buf_3 = vga_front.save_area(0, ZOOM_Y1, ZOOM_X1-1, ZOOM_Y2);
#ifdef USE_FLIP
		save_buf_3b = vga_back.save_area(0, ZOOM_Y1, ZOOM_X1-1, ZOOM_Y2);
#endif
	}
	else
	{
		save_buf_3 = NULL;
		save_buf_3b = NULL;
	}

	if( ZOOM_X2 < VGA_WIDTH-1 )
	{
		save_buf_4 = vga_front.save_area(ZOOM_X2+1, ZOOM_Y1, VGA_WIDTH-1, ZOOM_Y2);
#ifdef USE_FLIP
		save_buf_4b = vga_back.save_area(ZOOM_X2+1, ZOOM_Y1, VGA_WIDTH-1, ZOOM_Y2);
#endif
	}
	else
	{
		save_buf_4 = NULL;
		save_buf_4b = NULL;
	}
}
//---------- End of function Info::save_game_scr ---------//


//------- Begin of function Info::rest_game_scr --------//
//
void Info::rest_game_scr()
{
	// restore area outside front buffer
	mouse.hide();

#ifdef USE_FLIP
	if(save_buf_4b)
		vga_back.rest_area(save_buf_4b, 1);
	save_buf_4b = NULL;
	if(save_buf_3b)
		vga_back.rest_area(save_buf_3b, 1);
	save_buf_3b = NULL;
	if(save_buf_2b)
		vga_back.rest_area(save_buf_2b, 1);
	save_buf_2b = NULL;
#endif
	if(save_buf_4)
		vga_front.rest_area(save_buf_4, 1);
	save_buf_4 = NULL;
	if(save_buf_3)
		vga_front.rest_area(save_buf_3, 1);
	save_buf_3 = NULL;
	if(save_buf_2)
		vga_front.rest_area(save_buf_2, 1);
	save_buf_2 = NULL;
	if(save_buf_1)
		vga_front.rest_area(save_buf_1, 1);
	save_buf_1 = NULL;
	if(save_buf_1b)
		vga_back.rest_area(save_buf_1b, 1);
 	save_buf_1b = NULL;

	save_buf_1 = NULL;

	info.disp();
	sys.blt_virtual_buf();		// blt the virtual front buffer to the screen
	mouse.show();
}
//---------- End of function Info::rest_game_scr ---------//

//------- Begin of function Info::free_game_scr --------//
//
void Info::free_game_scr()
{
	if(save_buf_4)
	{
		mem_del(save_buf_4);
		save_buf_4 = NULL;
	}

	if(save_buf_3)
	{
		mem_del(save_buf_3);
		save_buf_3 = NULL;
	}

	if(save_buf_2)
	{
		mem_del(save_buf_2);
		save_buf_2 = NULL;
	}

	if(save_buf_1)
	{
		mem_del(save_buf_1);
		save_buf_1 = NULL;
	}

	if(save_buf_4b)
	{
		mem_del(save_buf_4b);
		save_buf_4b = NULL;
	}

	if(save_buf_3b)
	{
		mem_del(save_buf_3b);
		save_buf_3b = NULL;
	}

	if(save_buf_2b)
	{
		mem_del(save_buf_2b);
		save_buf_2b = NULL;
	}

	if(save_buf_1b)
	{
		mem_del(save_buf_1b);
		save_buf_1b = NULL;
	}
}
//---------- End of function Info::free_game_scr ---------//

//------- Begin of function Info::disp_loyalty --------//
//
// return: <int> x2 - the ending x position of the loyalty string.
//
int Info::disp_loyalty(int x, int y, int x2, int curLoyalty, int targetLoyalty, int nationRecno, int refreshFlag, int useBlueFont)
{
	int endX;

	if( x < x2 )		// if x==x2, don't display the field name.
//		(useBlueFont?font_blu2:font_snds).put( x, y, "Loyalty", 1, x2-1 );
		(useBlueFont?font_blu2:font_snds).put( x, y, text_unit.str_loyalty(), 1, x2-1 );

	endX = font_snds.put( x2, y, curLoyalty, 1 );		// display current loyalty

	if( nation_array[nationRecno]->cash <= 0 )		// if the nation no longer has money to pay the unit
		targetLoyalty = 0;

	if( targetLoyalty > curLoyalty )
	{
		// ##### begin Gilbert 22/12 #####//
		vga.active_buf->put_bitmap_trans( endX+3, y+2, image_icon.read("ARROWUP") );
		// ##### end Gilbert 22/12 #####//
		endX += 10;
		endX = font_snds.put( endX, y, targetLoyalty, 1 );
	}
	else if( targetLoyalty < curLoyalty )
	{
		// ##### begin Gilbert 22/12 #####//
		vga.active_buf->put_bitmap_trans( endX+3, y+2, image_icon.read("ARROWDWN") );
		// ##### end Gilbert 22/12 #####//
		endX += 10;
		endX = font_snds.put( endX, y, targetLoyalty, 1 );
	}

	return endX;
}
//---------- End of function Info::disp_loyalty ---------//

// --------- begin of function Info::draw_unit_icon -------//
//
// draw unit icon
// <int> x, y         screen coordinate to draw the icon
// <int> unitId       unit id
// <int> nationRecno  nation recno of the unit, to get color table, (negative for color scheme id)
// <int> windowX1,Y1,X2,Y2		clipping window
// <int> para			 0=draw icon only
//                    bit 0=draw icon
//                    bit 1=selected, draw blue frame
//                    bit 2=cursor on area, draw yellow frame
//                    bit 3=detect left button
//                    bit 4=detect right button
//							 bit 5=darken the icon
//							 other upper bits used as the level of darkness
//
// return para with some bit cleared
// if not detected, bit 3 cleared
// if cursor not on area, bit 2 cleared
//
int Info::draw_unit_icon( int x, int y, int unitId, int nationRecno, 
	int windowX1, int windowY1, int windowX2, int windowY2, int para)
{

	err_when( !unitId );
	UnitInfo *unitInfo = unit_res[unitId];
	Bitmap *bitmapPtr = (Bitmap *) unitInfo->unit_icon_ptr;
	int x1 = x + unitInfo->unit_icon_offsetx;
	int y1 = y + unitInfo->unit_icon_offsety;
	int x2 = x1 + bitmapPtr->get_width() -1;
	int y2 = y1 + bitmapPtr->get_height() -1;
	int j;

	// ###### begin Gilbert 12/3 ########//
	int colorSchemeId;
	if( nationRecno < 0 )			// colorSchemeId for use with campaign nation
		colorSchemeId = -nationRecno;
	else if( nation_array.is_deleted(nationRecno) )
		colorSchemeId = 0;
	else
		colorSchemeId = nation_array[nationRecno]->color_scheme_id;
	// ###### end Gilbert 12/3 ########//

	int colorArray[4] = { 0, VGA_LIGHT_BLUE, VGA_YELLOW, VGA_LIGHT_GREEN };

	// if whole outside, return 0

	if( x1 > windowX2 || x2 < windowX1 || y1 > windowY2 || y2 < windowY1 )
		return 0;

	//short *colorRemapTable = game.get_color_remap_table(nationRecno, para==1);
	short *colorRemapTable = game.get_color_remap_table(nationRecno, para&2);

	int rc = para;

	if( x1 >= windowX1 && x2 <= windowX2 && y1 >= windowY1 && y2 <= windowY2 )
	{
		// ------ completely inside --------//

		if( para & 1 )
		{
			// ------- draw icon --------//
			if( para & 2 )
			{
				// brighten on selected
				colorRemapTable = (short *)vga.vga_color_table->get_table(6);
				for( j=0 ; j<8 ; j++ )
					colour_buffer[j] = colorRemapTable[FIRST_REMAP_KEY+j];
				for( j=0 ; j<4 ; j++ )
					colorRemapTable[FIRST_REMAP_KEY+j] = colorRemapTable[remap_location_array[colorSchemeId*2]+j];
				for( j=0 ; j<4 ; j++ )
					colorRemapTable[FIRST_REMAP_KEY+4+j] = colorRemapTable[remap_location_array[colorSchemeId*2+1]+j];
			}
			else if( para & 32 )
			{
				// ------- darken the icon --------//
				colorRemapTable = (short *)vga.vga_color_table->get_table(-((para >> 6) +5));
				for( j=0 ; j<8 ; j++ )
					colour_buffer[j] = colorRemapTable[FIRST_REMAP_KEY+j];
								
			//	for( j=0 ; j<8 ; j++ )
			//		colorRemapTable[FIRST_REMAP_KEY+j] = (short) vga.make_pixel(customized_color_code[colorSchemeId*24+j*3],
			//			customized_color_code[colorSchemeId*24+j*3+1], customized_color_code[colorSchemeId*24+j*3+2]);
				for( j=0 ; j<4 ; j++ )
					colorRemapTable[FIRST_REMAP_KEY+j] = colorRemapTable[remap_location_array[colorSchemeId*2]+j];
				for( j=0 ; j<4 ; j++ )
					colorRemapTable[FIRST_REMAP_KEY+4+j] = colorRemapTable[remap_location_array[colorSchemeId*2+1]+j];
			}

			vga.active_buf->put_bitmap_trans_remap_decompress( x1, y1, (char *)bitmapPtr, colorRemapTable );
			if( para & 32 || para & 2 )
			{
				for( j=0 ; j<8 ; j++ )
					colorRemapTable[FIRST_REMAP_KEY+j] = colour_buffer[j];
			}
		}
		
		if( para & 6 )
		{
			// -------- frame -------- //

			int p = para & 6;
			if( (para & 4) && !mouse.in_area(x1, y1, x2, y2) )	// if not on cursor, clear bit1
			{
				rc &= ~4;
				p &= ~4;
			}
			if( p )				// draw rectangle
				vga.active_buf->rect( x1, y1, x2, y2, 1, colorArray[p >> 1]);
		}


		// ------- detect left button --------//

		if( para & 8 && 
			(mouse.any_click(x1, y1, x2, y2, LEFT_BUTTON) == 0 || 
			(game.game_mode == GAME_TUTORIAL && !tutor.allow_unit_icon_click(x1, y1, unitInfo, para & 32, 1))) )
		{
			rc &= ~8;			// clear bit 3
		}

		// ------- detect right button --------//

		if( para & 16 && 
			(mouse.any_click(x1, y1, x2, y2, RIGHT_BUTTON) == 0 || 
			(game.game_mode == GAME_TUTORIAL && !tutor.allow_unit_icon_click(x1, y1, unitInfo, para & 32, 2))) )
		
		{
			rc &= ~16;		// clear bit 4
		}

	}
	else
	{
		// partially inside
		int srcX1 = max(x1, windowX1)-x1;
		int srcY1 = max(y1, windowY1)-y1;
		int srcX2 = min(x2, windowX2)-x1;
		int srcY2 = min(y2, windowY2)-y1;

		if( para & 1 )
		{
			// ------ draw icon ----------//

			if( vga.active_buf->is_front )
				mouse.hide_area( x1+srcX1, y1+srcY1, x1+srcX2, x1+srcY2);

			if( para & 2 )
			{
				colorRemapTable = (short *)vga.vga_color_table->get_table(6);
				for( j=0 ; j<8 ; j++ )
					colour_buffer[j] = colorRemapTable[FIRST_REMAP_KEY+j];
				for( j=0 ; j<4 ; j++ )
					colorRemapTable[FIRST_REMAP_KEY+j] = colorRemapTable[remap_location_array[colorSchemeId*2]+j];
				for( j=0 ; j<4 ; j++ )
					colorRemapTable[FIRST_REMAP_KEY+4+j] = colorRemapTable[remap_location_array[colorSchemeId*2+1]+j];
			}
			else if( para & 32 )
			{
				// ------- darken the icon --------//
				colorRemapTable = (short *)vga.vga_color_table->get_table(-((para >> 6) +5));
				for( j=0 ; j<8 ; j++ )
					colour_buffer[j] = colorRemapTable[FIRST_REMAP_KEY+j];
								
			//	for( j=0 ; j<8 ; j++ )
			//		colorRemapTable[FIRST_REMAP_KEY+j] = (short) vga.make_pixel(customized_color_code[colorSchemeId*24+j*3],
			//			customized_color_code[colorSchemeId*24+j*3+1], customized_color_code[colorSchemeId*24+j*3+2]);
				for( j=0 ; j<4 ; j++ )
					colorRemapTable[FIRST_REMAP_KEY+j] = colorRemapTable[remap_location_array[colorSchemeId*2]+j];
				for( j=0 ; j<4 ; j++ )
					colorRemapTable[FIRST_REMAP_KEY+4+j] = colorRemapTable[remap_location_array[colorSchemeId*2+1]+j];
			}

			vga.active_buf->put_bitmap_area_trans_remap_decompress( x1, y1, (char *)bitmapPtr,
				srcX1, srcY1, srcX2, srcY2, colorRemapTable );

			if( para & 32 || para & 2 )
			{
				for( j=0 ; j<8 ; j++ )
					colorRemapTable[FIRST_REMAP_KEY+j] = colour_buffer[j];
			}

			if( vga.active_buf->is_front )
				mouse.show_area();
		}

		if( para & 6 )
		{
			// -------- frame -------- //

			int p = para & 6;
			if( (para & 4) && !mouse.in_area(x1, y1, x2, y2) )	// if not on cursor, clear bit1
			{
				rc &= ~4;
				p &= ~4;
			}
			if( p )				// draw rectangle
			{
				if( x1 >= windowX1 )		// draw left side
					vga.active_buf->bar( x1, y1+srcY1, x1, y1+srcY2, colorArray[p >> 1] );
				if( x2 <= windowX2 )		// draw right side
					vga.active_buf->bar( x2, y1+srcY1, x2, y1+srcY2, colorArray[p >> 1] );
				if( y1 >= windowY1 )		// draw top side
					vga.active_buf->bar( x1+srcX1, y1, x1+srcX2, y1, colorArray[p >> 1] );
				if( y2 <= windowY2 )		// draw bottom side
					vga.active_buf->bar( x1+srcX1, y2, x1+srcX2, y2, colorArray[p >> 1] );
			}
		}

		// ------- detect left button --------//

		if( para & 8 && 
			(mouse.any_click(x1+srcX1, y1+srcY1, x1+srcX2, y1+srcY2, LEFT_BUTTON) == 0 || 
			(game.game_mode == GAME_TUTORIAL && !tutor.allow_unit_icon_click(x1, y1, unitInfo, para & 32, 1))) )
		{
			rc &= ~8;		// clear bit 3
		}

		// ------- detect left button --------//

		if( para & 16 && 
			(mouse.any_click(x1+srcX1, y1+srcY1, x1+srcX2, y1+srcY2, RIGHT_BUTTON) == 0 || 
			(game.game_mode == GAME_TUTORIAL && !tutor.allow_unit_icon_click(x1, y1, unitInfo, para & 32, 2))) )
		{
			rc &= ~16;	// clear bit 4
		}
	}

	return rc;
}
// --------- end of function Info::draw_unit_icon -------//
