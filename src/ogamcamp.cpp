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


//Filename    : OGAMCAMP.CPP
//Description : Campaign related functions

#include <osys.h>
#include <ogame.h>
#include <ocampall.h>
#include <ogammenu.h>
#include <oprofile.h>
#include <obitmap.h>
#include <ovga.h>
#include <omodeid.h>
#include <ovgalock.h>
#include <ocoltbl.h>
#include <omouse.h> 
#include <oimgres.h>
#include <ofont.h>
#include <obuttcus.h>
#include <ogeta.h>
#include <omusic.h>
#include <ot_gmenu.h>
#include <ot_camp.h>


//---------- Define option modes --------//

enum { OPTION_CAMPAIGN,
		 OPTION_BASIC,
		 OPTION_ADVANCED 
};

// ----- declare static function -------//

static void i_disp_text_button(ButtonCustom *, int repaintBody);
static void i_disp_race_button(ButtonCustom *, int repaintBody);
static void i_disp_color_button(ButtonCustom *, int repaintBody);


//-------- Begin of function Game::run_campaign --------//
//
void Game::run_campaign()
{
	err_when( cur_campaign );

	Config saveConfig = config;		// save the original config as the campaign will overwrite it.

	int rc = select_campaign_menu();

	//---- while we save the original config, we need to overwrite it with new settings the player has just chosen ---//

	saveConfig.race_id = config.race_id;
	saveConfig.player_nation_color = config.player_nation_color;

	strcpy( saveConfig.player_name, config.player_name );

	saveConfig.campaign_difficulty = config.campaign_difficulty;
	saveConfig.building_size = config.building_size;

	saveConfig.fog_of_war = config.fog_of_war;
	saveConfig.random_event_frequency = config.random_event_frequency;

	//---- create debuggin log if in testing mode -----//

#ifdef DEBUG
	if( m.is_file_exist("CTEST.SYS") )
	{
		game.debug_log = new LongLog('0');
		game.debug_log->printf( "\n\r---------------------------\n\r" );
	}

	//---------------------------------------//

	char	total_stage_run_count_array[MAX_STAGE];
	char	total_event_run_count_array[MAX_EVENT];
	int   maxStage, maxEvent;

	memset( total_stage_run_count_array, 0, sizeof(total_stage_run_count_array) );
	memset( total_event_run_count_array, 0, sizeof(total_event_run_count_array) );
#endif

	//---------------------------------------//

	while( rc > 0 )
	{
		init_campaign(rc);

		cur_campaign->init_new_campaign();

		game.main_loop_all();

		int autoTestFlag = cur_campaign->auto_test_flag;

		//----- write debug log ---------//

#ifdef DEBUG
		if( game.debug_log )
		{
			game.debug_log->printf( "\n\r---------------------------\n\r" );

			//--- write the execution counters ----//

			for( int i=0 ; i<cur_campaign->max_stage ; i++ )
			{
				game.debug_log->printf( "Stage %d: %d\n\r", i+1, cur_campaign->stage_run_count_array[i] );		// stage run counters
				total_stage_run_count_array[i] += cur_campaign->stage_run_count_array[i];
			}

			game.debug_log->printf( "\n\r" );

			for( i=0 ; i<cur_campaign->max_event ; i++ )
			{
				game.debug_log->printf( "Event %d: %d\n\r", i+1, cur_campaign->event_run_count_array[i] );		// event run counters
				total_event_run_count_array[i] += cur_campaign->event_run_count_array[i];
			}

			maxStage = cur_campaign->max_stage;
			maxEvent = cur_campaign->max_event;

			game.debug_log->printf( "\n\r---------------------------\n\r" );
		}
#endif

		//-------------------------------------//

		deinit_all();

		if( autoTestFlag )
			rc = 1;
		else
			rc = 0;
	}

	config = saveConfig;

#ifdef DEBUG
	if( game.debug_log )
	{
		//-- write the total of the counters from all games previously run --//

		game.debug_log->printf( "TOTAL\n\r" );

		for( int i=0 ; i<maxStage ; i++ )
			game.debug_log->printf( "Stage %d: %d\n\r", i+1, total_stage_run_count_array[i] );		// stage run counters

		game.debug_log->printf( "\n\r" );

		for( i=0 ; i<maxEvent ; i++ )
			game.debug_log->printf( "Event %d: %d\n\r", i+1, total_event_run_count_array[i] );		// event run counters

		game.debug_log->printf( "\n\r---------------------------\n\r" );

		//------ delete the log class ------//

		delete game.debug_log;
		game.debug_log = NULL;
	}
#endif
}
//--------- End of function Game::run_campaign ---------//


//-------- Begin of function Game::init_campaign --------//
//
void Game::init_campaign(int campaignId)
{
	err_when( cur_campaign );

	switch( campaignId )
	{
		case CAMPAIGN_EAST_WEST:
			cur_campaign = new CampaignEastWest;
			break;

		default:
			err_here();
	}

	cur_campaign->init(campaignId);
}
//--------- End of function Game::init_campaign ---------//

//-------- Begin of function Game::deinit_campaign --------//
//
void Game::deinit_campaign()
{
	delete cur_campaign;
	cur_campaign = NULL;
}
//--------- End of function Game::deinit_campaign ---------//

//-------- Begin of function Game::campaign_class_size --------//
//
int Game::campaign_class_size(int campaignId)
{
	switch( campaignId )
	{
		case CAMPAIGN_EAST_WEST:
			return sizeof(CampaignEastWest);

		default:
			err_here();
			return 0;
	}
}
//--------- End of function Game::campaign_class_size ---------//


//-------- Begin of function Game::deinit_all --------//
//
// Handle the deinitialization of the game in both
// standalone game mode and campaign mode.
//
void Game::deinit_all()
{
	if( game.is_campaign_mode() )
		deinit_campaign();
	else
		deinit();
}
//--------- End of function Game::deinit_all ---------//


//-------- Begin of function Game::main_loop_all --------//
//
// [int] isLoadedGame - whether the game is a loaded game.
//							   (default: 0)
//
void Game::main_loop_all(int isLoadedGame)
{
	// ##### begin Gilbert 10/4 #######//
	// press alt-f4 during game
	if( sys.signal_exit_flag && !vga_front.vptr_dd_buf )
		return;
	// ##### end Gilbert 10/4 #######//

	sys.signal_exit_flag = 0;

	while(1)
	{
		if( is_campaign_mode() )
			campaign()->main_loop(isLoadedGame);
		else
			sys.run(isLoadedGame);

		if( sys.signal_exit_flag == 4 )		// 4-this is set after loading a game for exiting the current main_loop() so that it can enter the correct main_loop() (campaign & game) later.
		{
			isLoadedGame = 1;
			sys.signal_exit_flag = 0;
		}
		else
			break;
	}
}
//-------- End of function Game::main_loop_all --------//


// ------- Begin of function Game::select_campaign_menu -----//
//
// select which campaign to play, set config as well
//
// return 0=cancel, >0 for campaign selected
//
int Game::select_campaign_menu()
{
	// static char *campaignList[] = { "Occidental Heroes" };	//, "Human Learning Campaign", "Fryhtan Campaign", "Fryhtan Learning Campaign", };
	static char *campaignList[] = 
	{
		text_campaign.str_campaign_name(CAMPAIGN_EAST_WEST),
	};
	int campaignCount = sizeof(campaignList) / sizeof(campaignList[0]);

	Config tempConfig = config;

	// ------ adjust config ------//

	tempConfig.default_campaign_setting();

	if( tempConfig.race_id < 0 )		// human race only
		tempConfig.race_id = 1;

	// ----------------------------//

	int optionMode;
	if (campaignCount == 1)
		optionMode = OPTION_BASIC;
	else
		optionMode = OPTION_CAMPAIGN;

	TempGameSet tempGameSet(1);		// game_set.open_set
	TempUnitRes tempUnitRes;			// unit_res.init
	TempRaceRes tempRaceRes;			// race_res.init

	// take name from profile

	if( player_profile.is_registered() )
	{
		strcpy( tempConfig.player_name, player_profile.player_name );
	}

	//--------- initialize variable ---------//

	int i;
	int w, h;
	int cx, cy;
	String str;
	long refreshFlag = SGOPTION_ALL;
	int retFlag = 0;

	// -------- generate palette ------//

	short colorRemapTable[0x100];
	{
		str = DIR_IMAGE;
		str += "CHOOSE.COL";

		File palFile;
		palFile.file_open(str);
		ColorTable colorTable;

		BYTE palBuf[0x100][3];
		palFile.file_seek(8);     				// bypass the header info
		palFile.file_read(palBuf, sizeof(palBuf));
		palFile.file_close();

		// ------- palette description -------------//

		PalDesc palBufDesc( palBuf, 3, 0x100, 8 );

		//-------- create color remap table ---------//

		colorTable.generate_table_fast( 0, palBufDesc, ColorTable::bright_func );
		memcpy( colorRemapTable, colorTable.get_table(0), sizeof(colorRemapTable) );
	}

	// ------ initialize human / fryhtan button ------//

	ButtonCustomGroup campaignGroup(campaignCount);

	for( i = 0; i < campaignCount; ++i )
	{
		// divide 125, 680 into campaignCount partitions
		// cx is the center of each parition
		// campaignCount = 1 , (1:1)
		// campaignCount = 2 , (3:1), (1:3)
		// campaignCount = 3 , (5:1), (3:3), (1:5)
		//cx = ( 125*((campaignCount-i)*2-1) + 680*(i*2+1)) / (campaignCount*2);
		//cy = 136;
		cx = 390;
		cy = ( 136 * ((campaignCount-i)*2-1) + 280*(i*2+1)) / (campaignCount*2);

		w = font_thin_black.text_width( campaignList[i] );
		h = font_thin_black.text_height();
		campaignGroup[i].create( cx-w/2-10, cy-h/2-5, cx+w/2+10, cy+h/2+5,
		i_disp_text_button, ButtonCustomPara( campaignList[i], 0 ), 0 );
	}

	// -------- initialize player name field ------//

	GetA playerNameField;
	playerNameField.init( 364, 89, 688, tempConfig.player_name,
		tempConfig.PLAYER_NAME_LEN, &font_bold_black, 0, 1 );

	// -------- initialize human group ---------//

	ButtonCustomGroup raceGroup(MAX_RACE);

	err_when( !race_res.init_flag );
	cx = 210;
	cy = 160;
	const raceGroupYSpacing = 22;
	for( i = 0; i < MAX_RACE; ++i, (cy += raceGroupYSpacing) )
	{
		w = font_thin_black.text_width( race_res[i+1]->name );
		h = raceGroupYSpacing;
		raceGroup[i].create( cx-w/2-5, cy-h/2, cx+w/2+5, cy+h/2,
			i_disp_race_button, ButtonCustomPara( NULL, i+1 ), 0 );
	}

	// ------- create color group --------//

	ButtonCustomGroup colorGroup(MAX_COLOR_SCHEME);
	const colorButtonFrameX = 382;
	const colorButtonFrameY = 155;
	cx = 391;
	cy = 162;
	const colorButtonWidth = 29;
	const colorButtonHeight = 30;
	for(i = 0; i < MAX_COLOR_SCHEME; ++i, (cx+=colorButtonWidth) )
	{
		colorGroup[i].create( cx, cy, cx+colorButtonWidth-1, cy+colorButtonHeight-1, 
			i_disp_color_button, ButtonCustomPara(NULL, i+1), 0 );
	}

	// ---------- initialize campaign difficulty_level button group -------//

	ButtonCustomGroup campDiffGroup(5);
	char campDiffButtonStr[5][10];
	cx = 410;
	cy = 331;
	for( i = 0; i < 5; ++i )
	{
		strcpy( campDiffButtonStr[i], m.roman_number(i+1) );

		w = font_thin_black.text_width(campDiffButtonStr[i]);
		h = font_thin_black.text_height();
		campDiffGroup[i].create( cx, cy, cx+w+10, cy+h+10,
			i_disp_text_button, ButtonCustomPara(campDiffButtonStr[i], i+1), 0);
		cx += w + 20;
	}

	// --------- initialize building size --------//

	ButtonCustomGroup buildingSizeGroup(2);
	char *buildingSizeButtonStr[2] =
	{
		text_game_menu.str_building_set(1),
		text_game_menu.str_building_set(2),
	};
	cx = 370+63;
	cy = 386;
	for( i = 0; i < buildingSizeGroup.button_num; ++i )
	{
		w = font_thin_black.text_width(buildingSizeButtonStr[i]);
		h = font_thin_black.text_height();
		buildingSizeGroup[i].create( cx, cy, cx+w+10, cy+h+10, 
			i_disp_text_button, ButtonCustomPara(buildingSizeButtonStr[i], i+1), 0 );
		cx += w + 12;
	}

	// ------ initialize fog of war ------//

	const option3X = 320;
	ButtonCustomGroup fogGroup(2);
	cx = option3X;
	cy = 173; // cy = 143;
	w = font_thin_black.text_width( text_game_menu.str_off_on(0) );	// Off
	h = font_thin_black.text_height();
	fogGroup[0].create( cx, cy, cx+w+10, cy+h+10,
		i_disp_text_button, ButtonCustomPara( text_game_menu.str_off_on(0), 0), 0 );		// Off
	cx += w+20;
	w = font_thin_black.text_width( text_game_menu.str_off_on(1) );	// On
	fogGroup[1].create( cx, cy, cx+w+10, cy+h+10,
		i_disp_text_button, ButtonCustomPara( text_game_menu.str_off_on(1), 1), 0 );	// On

/*
	// --------- initialize spy methodology-------//

	char *spyMethodStr[2] = { "Must  Research", "All  Available" };

	ButtonCustomGroup spyMethodGroup(2);
	cx = option3X-10;
	cy = 348;
	for( i = 0; i < 2; ++i )
	{
		w = font_thin_black.text_width( spyMethodStr[i] );
		h = font_thin_black.text_height();
		spyMethodGroup[i].create( cx, cy, cx+w+10, cy+h+10,
			i_disp_text_button, ButtonCustomPara( spyMethodStr[i], i), 0 );
		cx += w+20;
	}
*/
	// --------- initialize random_event_frequency button group --------//

	char *randomEventStr[4] =
	{
		text_game_menu.str_never_to_frequent(OPTION_NONE),
		text_game_menu.str_never_to_frequent(OPTION_LOW),
		text_game_menu.str_never_to_frequent(OPTION_MODERATE),
		text_game_menu.str_never_to_frequent(OPTION_HIGH),
	};

	ButtonCustomGroup randomEventGroup(4);
	cx = option3X;
	cy = 314; // cy = 384;
	for( i = 0; i < 4; ++i )
	{
		w = font_thin_black.text_width( randomEventStr[i] );
		h = font_thin_black.text_height();
		randomEventGroup[i].create(cx, cy, cx+w+10, cy+h+6,
			i_disp_text_button, ButtonCustomPara(randomEventStr[i], i), 0);
		if( (i+1) % 2 == 0 )
		{
			// line feed
			cx = option3X;
			cy += 26;
		}
		else
		{
			cx += 144;
		}
	}


	// ------- loop ---------//

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
					return 0;
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
				refreshFlag = SGOPTION_ALL;
				sys.need_redraw_flag = 0;
			}

			VgaFrontReLock vgaReLock;

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

			// -------- display ----------//

			if( refreshFlag )
			{
				if( optionMode == OPTION_CAMPAIGN )
				{
					if( refreshFlag & SGOPTION_PAGE )
					{
						vga.use_back();
						vga.disp_image_file("CHOOSE");

						// ------- display option Mode ------//

						for( i = OPTION_CAMPAIGN; i <= OPTION_ADVANCED; ++i )
						{
							// red font for selected
							Font *fontPtr = (i == optionMode ? &font_bold_red : &font_bold_black );

							fontPtr->center_put(OPTION_SWITCH_X1, OPTION_SWITCH_Y1+OPTION_SWITCH_Y_SPACING*i,
								OPTION_SWITCH_X2, OPTION_SWITCH_Y2+OPTION_SWITCH_Y_SPACING*i, 
								m.roman_number(i+1) );
						}

						// ----- display start, cancel button ------//

						font_thin_black.center_put( BUTTON2_X1, BUTTON2_Y1, BUTTON2_X2, BUTTON2_Y2,
							text_game_menu.str_start() );
						font_thin_black.center_put( BUTTON4_X1, BUTTON4_Y1, BUTTON4_X2, BUTTON4_Y2,
							text_game_menu.str_cancel() );

						vga.use_front();
						vga.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
					}

					if( refreshFlag & SGOPTION_CAMPAIGN )
						campaignGroup.paint();
				}

				// ------- display basic option ---------//

				if( optionMode == OPTION_BASIC )
				{
					if( refreshFlag & SGOPTION_PAGE )
					{
						vga.use_back();
						vga.disp_image_file("CHOOSE");
						// BUGHERE : option menu column and finger

						font_bold_black.right_put( playerNameField.x, playerNameField.y,
							text_game_menu.str_king_name() );
						font_bold_black.center_put( 116, 126, 303, 146,	text_game_menu.str_nationality() );
						font_bold_black.center_put( 382, 129, 600, 149, text_game_menu.str_color() );
						font_bold_black.center_put( 341, 305, 654, 324, text_game_menu.str_difficulty_level() );
						font_bold_black.center_put( 341, 365, 660, 384, text_game_menu.str_building_set() );
						
						// ------- display option Mode ------//

						for( i = OPTION_CAMPAIGN; i <= OPTION_ADVANCED; ++i )
						{
							// red font for selected
							Font *fontPtr = (i == optionMode ? &font_bold_red : &font_bold_black );

							fontPtr->center_put(OPTION_SWITCH_X1, OPTION_SWITCH_Y1+OPTION_SWITCH_Y_SPACING*i,
								OPTION_SWITCH_X2, OPTION_SWITCH_Y2+OPTION_SWITCH_Y_SPACING*i, 
								m.roman_number(i+1) );
						}

						// ----- display start, cancel button ------//

						font_thin_black.center_put( BUTTON2_X1, BUTTON2_Y1, BUTTON2_X2, BUTTON2_Y2,
							text_game_menu.str_start() );
						font_thin_black.center_put( BUTTON4_X1, BUTTON4_Y1, BUTTON4_X2, BUTTON4_Y2,
							text_game_menu.str_cancel() );

						vga.use_front();
						vga.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
					}

					if( refreshFlag & SGOPTION_RACE )
						raceGroup.paint( tempConfig.race_id-1 );
					if( refreshFlag & SGOPTION_COLOR )
					{
						vga.use_back();		// to avoid flickering

						// ------ put color box ------ //
						char *bitmapPtr = image_button.read("F-COLOR");
						vga.active_buf->put_bitmap_trans_remap_decompress(
							colorButtonFrameX, colorButtonFrameY, bitmapPtr,
							game.color_remap_array[tempConfig.player_nation_color].color_table );
						colorGroup.paint(tempConfig.player_nation_color-1);

						vga.use_front();

						vga.blt_buf( colorButtonFrameX, colorButtonFrameY,
							colorButtonFrameX + ((Bitmap *)bitmapPtr)->get_width() - 1,
							colorButtonFrameY + ((Bitmap *)bitmapPtr)->get_height() - 1, 0 );
					}
					if( refreshFlag & SGOPTION_DIFFICULTY )
						campDiffGroup.paint(tempConfig.campaign_difficulty-1);
					if( refreshFlag & SGOPTION_BUILDING_SIZE )
						buildingSizeGroup.paint(tempConfig.building_size-1);
					if( refreshFlag & SGOPTION_NAME_FIELD )
						playerNameField.paint();
				}

				// ------- display advanced option ---------//
				if( optionMode == OPTION_ADVANCED )
				{
					if( refreshFlag & SGOPTION_PAGE )
					{
						vga.use_back();
						vga.disp_image_file("CHOOSE");

						font_bold_black.put_paragraph( 126, 173, option3X-10, 213-1,
							text_game_menu.str_fog_of_war() );

//						font_bold_black.put_paragraph( 126, 339, option3X-10, 389-1,
//							text_game_menu.str_spy_methodology() );

						font_bold_black.put_paragraph( 126, 314, option3X-10, 364-1,
							text_game_menu.str_random_events() );

						// ------- display option Mode ------//

						for( i = OPTION_CAMPAIGN; i <= OPTION_ADVANCED; ++i )
						{
							// red font for selected
							Font *fontPtr = (i == optionMode ? &font_bold_red : &font_bold_black );

							fontPtr->center_put(OPTION_SWITCH_X1, OPTION_SWITCH_Y1+OPTION_SWITCH_Y_SPACING*i,
								OPTION_SWITCH_X2, OPTION_SWITCH_Y2+OPTION_SWITCH_Y_SPACING*i,
								m.roman_number(i+1) );
						}

						// ----- display start, cancel button ------//

						font_thin_black.center_put( BUTTON2_X1, BUTTON2_Y1, BUTTON2_X2, BUTTON2_Y2,
							text_game_menu.str_start() );
						font_thin_black.center_put( BUTTON4_X1, BUTTON4_Y1, BUTTON4_X2, BUTTON4_Y2,
							text_game_menu.str_cancel() );

						vga.use_front();
						vga.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
					}
					if( refreshFlag & SGOPTION_FOG )
						fogGroup.paint(tempConfig.fog_of_war);

//					if( refreshFlag & SGOPTION_SPY_METHOD )
//						spyMethodGroup.paint(tempConfig.spy_methodology);

					if( refreshFlag & SGOPTION_RANDOM_EVENT )
						randomEventGroup.paint(tempConfig.random_event_frequency);
				}

				refreshFlag = 0;
			}

			sys.blt_virtual_buf();

			if( config.music_flag )
			{
				if( !music.is_playing(3) )
					music.play(3, sys.cdrom_drive ? MUSIC_CD_THEN_WAV : 0 );
			}
			else
				music.stop();

			// ------- detect --------//

			if( optionMode == OPTION_CAMPAIGN )
			{
				if( campaignGroup.detect() >= 0 
					|| campaignGroup[campaignGroup()].detect(0,0,0,1) )	// detect pushed button, but suspend pop
				{
					optionMode = OPTION_BASIC;		// auto change to basic mode
					refreshFlag = SGOPTION_ALL;
				}
			}

			else if( optionMode == OPTION_BASIC )
			{
				if( raceGroup.detect() >= 0)
				{
					tempConfig.race_id = raceGroup[raceGroup()].custom_para.value;
					//refreshFlag |= SGOPTION_RACE;
				}
				else if( colorGroup.detect() >= 0)
				{
					tempConfig.player_nation_color = colorGroup[colorGroup()].custom_para.value;
					refreshFlag |= SGOPTION_COLOR;
				}
				else if( campDiffGroup.detect() >= 0)
				{
					tempConfig.campaign_difficulty = campDiffGroup[campDiffGroup()].custom_para.value;
					refreshFlag |= SGOPTION_DIFFICULTY;
				}
				else if( buildingSizeGroup.detect() >= 0)
				{
					tempConfig.building_size = buildingSizeGroup[buildingSizeGroup()].custom_para.value;
					//refreshFlag |= SGOPTION_BUILDING_SIZE;
				}
				else if( playerNameField.detect() )
				{
					refreshFlag |= SGOPTION_NAME_FIELD;
				}
			}

			else if( optionMode == OPTION_ADVANCED )
			{
				if( fogGroup.detect() >= 0 )
				{
					tempConfig.fog_of_war = fogGroup[fogGroup()].custom_para.value;
					// refreshFlag |= SGOPTION_FOG;
				}
/*
				else if( spyMethodGroup.detect() >= 0 )
				{
					tempConfig.spy_methodology = spyMethodGroup[spyMethodGroup()].custom_para.value;
					// refreshFlag |= SGOPTION_SPY_METHOD;
				}
*/
				else if( randomEventGroup.detect() >= 0)
				{
					tempConfig.random_event_frequency = randomEventGroup[randomEventGroup()].custom_para.value;
					refreshFlag |= SGOPTION_RANDOM_EVENT;
				}
			}

			// -------- detect switch option button ---------//

			for( i = OPTION_CAMPAIGN; i <= OPTION_ADVANCED; ++i )
			{
				if( mouse.single_click( OPTION_SWITCH_X1, OPTION_SWITCH_Y1+OPTION_SWITCH_Y_SPACING*i,
					OPTION_SWITCH_X2, OPTION_SWITCH_Y2+OPTION_SWITCH_Y_SPACING*i) )
				{
					optionMode = i;
					refreshFlag = SGOPTION_ALL;
				}
			}

			// -------- detect start button --------//

			if( mouse.single_click( BUTTON2_X1, BUTTON2_Y1, BUTTON2_X2, BUTTON2_Y2 ) )
			{
				retFlag = campaignGroup() + 1;
				break;			// break  while(1)
			}

			// -------- detect cancel button --------//

			if( mouse.single_click( BUTTON4_X1, BUTTON4_Y1, BUTTON4_X2, BUTTON4_Y2 ) )
			{
				retFlag = 0;
				break;			// break  while(1)
			}

		}	// end while
	} // end of scope of VgaLock

	if( retFlag )
	{
		tempConfig.human_difficulty_rating = tempConfig.single_player_difficulty(1);
		tempConfig.monster_difficulty_rating = tempConfig.single_player_difficulty(-1);
		config = tempConfig;
	}

	return retFlag;
}
// ------- end of function Game::select_campaign_menu -----//



// ------ begin of static function i_disp_text_button ------//
//
// BustomCustom callback function to display text on top centre
//
static void i_disp_text_button(ButtonCustom *button, int repaintBody)
{
	if( !vga.use_back_buf )
		vga.blt_buf( button->x1, button->y1, button->x2, button->y2, 0 );
	Font *fontPtr = button->pushed_flag ? &font_bold_red : &font_thin_black;
	// top center align
	fontPtr->center_put( button->x1, button->y1, button->x2, button->y1+fontPtr->font_height-1,
		(char *)button->custom_para.ptr );
}
// ------ end of static function i_disp_text_button ------//


// ------ begin of static function i_disp_race_button ------//
//
// BustomCustom callback function to display race name on top centre
//
static void i_disp_race_button(ButtonCustom *button, int repaintBody)
{
	int raceId = button->custom_para.value;
	if( !vga.use_back_buf )
		vga.blt_buf( button->x1, button->y1, button->x2, button->y2, 0 );
	Font *fontPtr = button->pushed_flag ? &font_bold_red : &font_thin_black;
	// top center align
	fontPtr->center_put( button->x1, button->y1, button->x2, button->y1+fontPtr->font_height-1,
		raceId >= 0 ? race_res[raceId]->name : monster_res[-raceId]->name );
}
// ------ end of static function i_disp_race_button ------//


// ------ begin of static function i_disp_color_button ------//
//
// BustomCustom callback function to display color code marker
//
static void i_disp_color_button(ButtonCustom *button, int repaintBody)
{
	// 9,7,35,37, x+29
	int colorSchemeId = button->custom_para.value;
	short *colorRemapTable = game.color_remap_array[colorSchemeId].color_table;

	if( !button->pushed_flag )
	{
		int clipX1 = 9 + (colorSchemeId-1)*29;
		int clipY1 = 7;
		int clipX2 = clipX1 + 27 - 1;
		int clipY2 = 37;

		vga.active_buf->put_bitmap_area_trans_remap_decompress( 
			button->x1 - clipX1,	button->y1 - clipY1, image_button.read("F-COLOR"), 
			clipX1, clipY1, clipX2, clipY2,	colorRemapTable );
	}
	else
	{
		vga.active_buf->put_bitmap_trans_remap_decompress(
			button->x1, button->y1, image_button.read("F-COL-DW"), colorRemapTable );

	}
}
// ------ end of static function i_disp_color_button ------//
