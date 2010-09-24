
//Filename    : OGAMTRAN.CPP
//Description : Learning Campaign related functions

#include <OSYS.H>
#include <OGAME.H>
#include <OCAMPALL.H>
#include <OGAMMENU.H>
#include <OPROFILE.H>
#include <OBITMAP.H>
#include <OVGA.H>
#include <OMODEID.H>
#include <OVGALOCK.H>
#include <OCOLTBL.H>
#include <OMOUSE.H> 
#include <OIMGRES.H>
#include <OFONT.H>
#include <OBUTTCUS.H>
#include <OGETA.H>
#include <OMUSIC.H>


//---------- Define option modes --------//

enum { OPTION_LEARNING_CAMPAIGN,
		 OPTION_SPECIES,
	//	 OPTION_BASIC,		 
};

// ----- declare static function -------//

static void i_disp_text_button(ButtonCustom *, int repaintBody);
static void i_disp_race_button(ButtonCustom *, int repaintBody);
static void i_disp_color_button(ButtonCustom *, int repaintBody);


//-------- Begin of function Game::run_learning_campaign --------//
//
void Game::run_learning_campaign()
{
	err_when( cur_learning_campaign );

	Config saveConfig = config;		// save the original config as the campaign will overwrite it.

	int rc = select_learning_campaign_menu();

	//---- while we save the original config, we need to overwrite it with new settings the player has just chosen ---//
/*
	saveConfig.race_id = config.race_id;
	saveConfig.player_nation_color = config.player_nation_color;

	strcpy( saveConfig.player_name, config.player_name );

	saveConfig.campaign_difficulty = 2;//config.campaign_difficulty;
	saveConfig.building_size = config.building_size;

	saveConfig.fog_of_war = 0; //config.fog_of_war;
	saveConfig.random_event_frequency = 0; //config.random_event_frequency;

	//---- create debuggin log if in testing mode -----//

#ifdef DEBUG
	if( m.is_file_exist("CTEST.SYS") )
	{
		game.debug_log = new LongLog('0');
		game.debug_log->printf( "\n\r---------------------------\n\r" );
	}
#endif
*/
	//---------------------------------------//

	while( rc > 0 )
	{
		init_learning_campaign(rc);

		cur_learning_campaign->init_new_campaign();

		game.main_loop_all();

		int autoTestFlag = cur_learning_campaign->auto_test_flag;

		deinit_all();

		if( autoTestFlag )
			rc = 1;
		else
			rc = 0;

		//---------------------------------//
/*
#ifdef DEBUG
		if( game.debug_log )
			game.debug_log->printf( "\n\r---------------------------\n\r" );
#endif*/
	}

/*	config = saveConfig;

#ifdef DEBUG
	if( game.debug_log )
	{
		delete game.debug_log;
		game.debug_log = NULL;
	}
#endif*/
}
//--------- End of function Game::run_learning_campaign ---------//


//-------- Begin of function Game::init_learning_campaign --------//
//
void Game::init_learning_campaign(int learningCampaignId)
{
	err_when( cur_learning_campaign );

	switch( learningCampaignId )
	{
		case LEARNING_CAMPAIGN:
			cur_learning_campaign = new Campaign;
			break;

		default:
			err_here();
	}

	cur_learning_campaign->init(learningCampaignId);
}
//--------- End of function Game::init_learning_campaign ---------//

//-------- Begin of function Game::deinit_learning_campaign --------//
//
void Game::deinit_learning_campaign()
{
	delete cur_learning_campaign;
	cur_learning_campaign = NULL;
}
//--------- End of function Game::deinit_learning_campaign ---------//

//-------- Begin of function Game::learning_campaign_class_size --------//
//
int Game::learning_campaign_class_size(int learningCampaignId)
{
	switch( learningCampaignId )
	{
		case LEARNING_CAMPAIGN:
			return sizeof(Campaign);

		default:
			err_here();
			return 0;
	}
}
//--------- End of function Game::learning_campaign_class_size ---------//

/*
//-------- Begin of function Game::deinit_all --------//
//
// Handle the deinitialization of the game in both
// standalone game mode and campaign mode.
//
void Game::deinit_all()
{
	if( game.is_learning_campaign_mode() )
		deinit_learning_campaign();
	else
		deinit();
}
//--------- End of function Game::deinit_all ---------//
*/
/*
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
		if( is_learning_campaign_mode() )
			learning_campaign()->main_loop(isLoadedGame);
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
*/

// ------- Begin of function Game::select_learning_campaign_menu -----//
//
// select which campaign to play, set config as well
//
// return 0=cancel, >0 for campaign selected
//
int Game::select_learning_campaign_menu()
{

	static char *learningCampaignList[] = { "Military", "Economics" };//, "Fryhtan Campaign", "Fryhtan Learning Campaign", };
	int learningCampaignCount = sizeof(learningCampaignList) / sizeof(learningCampaignList[0]);

	Config tempConfig = config;

	// ------ adjust config ------//

	tempConfig.default_campaign_setting();

	if( tempConfig.race_id < 0 )		// human race only
		tempConfig.race_id = 1;

	// ----------------------------//

	int optionMode = OPTION_LEARNING_CAMPAIGN;

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

	// ------ initialize learning campaign selection ------//

	ButtonCustomGroup learningCampaignGroup(learningCampaignCount);

	for( i = 0; i < learningCampaignCount; ++i )
	{
		// divide 125, 680 into campaignCount partitions
		// cx is the center of each parition
		// campaignCount = 1 , (1:1)
		// campaignCount = 2 , (3:1), (1:3)
		// campaignCount = 3 , (5:1), (3:3), (1:5)
		//cx = ( 125*((campaignCount-i)*2-1) + 680*(i*2+1)) / (campaignCount*2);
		//cy = 136;
		cx = 390;
		cy = ( 136 * ((learningCampaignCount-i)*2-1) + 280*(i*2+1)) / (learningCampaignCount*2);

		w = font_thin_black.text_width( learningCampaignList[i] );
		h = font_thin_black.text_height();
		learningCampaignGroup[i].create( cx-w/2-10, cy-h/2-5, cx+w/2+10, cy+h/2+5,
		i_disp_text_button, ButtonCustomPara( learningCampaignList[i], 0 ), 0 );
	}

	// -------- initialize player name field ------//

	GetA playerNameField;
	playerNameField.init( 364, 89, 688, tempConfig.player_name,
		tempConfig.PLAYER_NAME_LEN, &font_bold_black, 0, 1 );

	// ------ initialize human / fryhtan button ------//
	ButtonCustomGroup speciesGroup(2);
	
	w = font_thin_black.text_width( "Humans" );
	h = font_thin_black.text_height();
	cx = 256;
	cy = 116;
	speciesGroup[0].create( cx-w/2-10, cy-h/2-5, cx+w/2+10, cy+h/2+5,
		i_disp_text_button, ButtonCustomPara( "Humans", 0 ), 0 );

	w = font_thin_black.text_width( "Fryhtans" );
	h = font_thin_black.text_height();
	cx = 532;
	cy = 116;
	speciesGroup[1].create( cx-w/2-10, cy-h/2-5, cx+w/2+10, cy+h/2+5,
		i_disp_text_button, ButtonCustomPara( "Fryhtans", 1 ), 0 );


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

	// --------- initialize monster group -------//

	ButtonCustomGroup monsterRaceGroup(MAX_MONSTER_TYPE);		// MAX_MONSTER
	err_when( !monster_res.init_flag );
	cx = 210;
	cy = 160;
	const monsterGroupYSpacing = 22;
	for( i = 0; i < MAX_MONSTER_TYPE; ++i, (cy += monsterGroupYSpacing) )		// MAX_MONSTER
	{
		w = font_thin_black.text_width( monster_res[i+1]->name );
		h = monsterGroupYSpacing;
		monsterRaceGroup[i].create( cx-w/2-5, cy-h/2, cx+w/2+5, cy+h/2,
			i_disp_race_button, ButtonCustomPara( NULL, -(i+1) ), 0 );
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

	// ---------- initialize learning campaign difficulty_level button group -------//

/*	ButtonCustomGroup learningCampDiffGroup(5);
	char learningCampDiffButtonStr[5][10];
	cx = 410;
	cy = 331;
	for( i = 0; i < 5; ++i )
	{
		strcpy( learningCampDiffButtonStr[i], m.roman_number(i+1) );

		w = font_thin_black.text_width(learningCampDiffButtonStr[i]);
		h = font_thin_black.text_height();
		learningCampDiffGroup[i].create( cx, cy, cx+w+10, cy+h+10,
			i_disp_text_button, ButtonCustomPara(learningCampDiffButtonStr[i], i+1), 0);
		cx += w + 20;
	}*/

	// --------- initialize building size --------//

	ButtonCustomGroup buildingSizeGroup(2);
	char *buildingSizeButtonStr[2] = { "Large", "Small" };
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
/*	ButtonCustomGroup fogGroup(2);
	cx = option3X;
	cy = 143;
	w = font_thin_black.text_width( "Off" );
	h = font_thin_black.text_height();
	fogGroup[0].create( cx, cy, cx+w+10, cy+h+10,
		i_disp_text_button, ButtonCustomPara( "Off", 0), 0 );
	cx += w+20;
	w = font_thin_black.text_width( "On" );
	fogGroup[1].create( cx, cy, cx+w+10, cy+h+10,
		i_disp_text_button, ButtonCustomPara( "On", 1), 0 );*/
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

	char *randomEventStr[4] = { "Never", "Seldom", "Occasional", "Frequent" };

	ButtonCustomGroup randomEventGroup(4);
	cx = option3X;
	cy = 384;
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
				music.stop();


			// -------- display ----------//

			if( refreshFlag )
			{
				if( optionMode == OPTION_SPECIES )
				{
					if( refreshFlag & SGOPTION_PAGE )
					{
						vga.use_back();
						vga.disp_image_file("CHOOSE");

						// ----- display picture -------//

						const pictureWidth = 298;
						const pictureHeight = 300;

						if( !vga.use_back_buf )
						{
							image_interface.put_front( (SCROLL_SHEET_X1+SCROLL_SHEET_X2)/2-pictureWidth, 
								SCROLL_SHEET_Y2-pictureHeight, "HUMANS" );
							image_interface.put_front( (SCROLL_SHEET_X1+SCROLL_SHEET_X2)/2, 
								SCROLL_SHEET_Y2-pictureHeight, "FRYHTANS" );
						}
						else
						{
							image_interface.put_back( (SCROLL_SHEET_X1+SCROLL_SHEET_X2)/2-pictureWidth, 
								SCROLL_SHEET_Y2-pictureHeight, "HUMANS" );
							image_interface.put_back( (SCROLL_SHEET_X1+SCROLL_SHEET_X2)/2, 
								SCROLL_SHEET_Y2-pictureHeight, "FRYHTANS" );
						}

						// ----- display start, cancel button ------//

						font_thin_black.center_put( BUTTON2_X1, BUTTON2_Y1, BUTTON2_X2, BUTTON2_Y2,
							"Start" );
						font_thin_black.center_put( BUTTON4_X1, BUTTON4_Y1, BUTTON4_X2, BUTTON4_Y2,
							"Cancel" );

						for( i = OPTION_LEARNING_CAMPAIGN; i <= OPTION_SPECIES; ++i )
						{
							// red font for selected
							Font *fontPtr = (i == optionMode ? &font_bold_red : &font_bold_black );

							fontPtr->center_put(OPTION_SWITCH_X1, OPTION_SWITCH_Y1+OPTION_SWITCH_Y_SPACING*i,
								OPTION_SWITCH_X2, OPTION_SWITCH_Y2+OPTION_SWITCH_Y_SPACING*i, 
								m.roman_number(i+1) );
						}

						vga.use_front();
						vga.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
					}
					if( refreshFlag & SGOPTION_RACE )
						speciesGroup.paint( tempConfig.race_id < 0 );

					// other ...
				}

				if( optionMode == OPTION_LEARNING_CAMPAIGN )
				{
					if( refreshFlag & SGOPTION_PAGE )
					{
						vga.use_back();
						vga.disp_image_file("CHOOSE");

						// ------- display option Mode ------//

						for( i = OPTION_LEARNING_CAMPAIGN; i <= OPTION_SPECIES; ++i )
						{
							// red font for selected
							Font *fontPtr = (i == optionMode ? &font_bold_red : &font_bold_black );

							fontPtr->center_put(OPTION_SWITCH_X1, OPTION_SWITCH_Y1+OPTION_SWITCH_Y_SPACING*i,
								OPTION_SWITCH_X2, OPTION_SWITCH_Y2+OPTION_SWITCH_Y_SPACING*i, 
								m.roman_number(i+1) );
						}

						// ----- display start, cancel button ------//

						font_thin_black.center_put( BUTTON2_X1, BUTTON2_Y1, BUTTON2_X2, BUTTON2_Y2,
							"Start" );
						font_thin_black.center_put( BUTTON4_X1, BUTTON4_Y1, BUTTON4_X2, BUTTON4_Y2,
							"Cancel" );

						vga.use_front();
						vga.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
					}

					if( refreshFlag & SGOPTION_CAMPAIGN )
						learningCampaignGroup.paint();
				}

/*				// ------- display basic option ---------//

				if( optionMode == OPTION_BASIC )
				{
					if( refreshFlag & SGOPTION_PAGE )
					{
						vga.use_back();
						vga.disp_image_file("CHOOSE");
						// BUGHERE : option menu column and finger

						font_bold_black.right_put( playerNameField.x, playerNameField.y,
							"King " );
						font_bold_black.center_put( 116, 126, 303, 146,	"Nationality");
						font_bold_black.center_put( 382, 129, 600, 149, "Color" );
					//	font_bold_black.center_put( 341, 305, 654, 324, "Difficulty Level" );
						font_bold_black.center_put( 341, 365, 660, 384, "Building Size" );
						
						// ------- display option Mode ------//

						for( i = OPTION_LEARNING_CAMPAIGN; i <= OPTION_SPECIES; ++i )
						{
							// red font for selected
							Font *fontPtr = (i == optionMode ? &font_bold_red : &font_bold_black );

							fontPtr->center_put(OPTION_SWITCH_X1, OPTION_SWITCH_Y1+OPTION_SWITCH_Y_SPACING*i,
								OPTION_SWITCH_X2, OPTION_SWITCH_Y2+OPTION_SWITCH_Y_SPACING*i, 
								m.roman_number(i+1) );
						}

						// ----- display start, cancel button ------//

						font_thin_black.center_put( BUTTON2_X1, BUTTON2_Y1, BUTTON2_X2, BUTTON2_Y2,
							"Start" );
						font_thin_black.center_put( BUTTON4_X1, BUTTON4_Y1, BUTTON4_X2, BUTTON4_Y2,
							"Cancel" );

						vga.use_front();
						vga.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
					}

					if( refreshFlag & SGOPTION_RACE )
					{
						if( tempConfig.race_id >= 0 )
							raceGroup.paint( tempConfig.race_id-1 );
						else
							monsterRaceGroup.paint( -tempConfig.race_id-1 );
					}
					
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
				//	if( refreshFlag & SGOPTION_DIFFICULTY )
				//		learningCampDiffGroup.paint(tempConfig.campaign_difficulty-1);
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

						font_bold_black.put_paragraph( 126, 143, option3X-10, 183-1,
							"Fog of War" );

						font_bold_black.put_paragraph( 126, 339, option3X-10, 389-1,
							"Espionage Methodologies" );

						font_bold_black.put_paragraph( 126, 384, option3X-10, 434-1,
							"Random Events" );

						// ------- display option Mode ------//

						for( i = OPTION_LEARNING_CAMPAIGN; i <= OPTION_ADVANCED; ++i )
						{
							// red font for selected
							Font *fontPtr = (i == optionMode ? &font_bold_red : &font_bold_black );

							fontPtr->center_put(OPTION_SWITCH_X1, OPTION_SWITCH_Y1+OPTION_SWITCH_Y_SPACING*i,
								OPTION_SWITCH_X2, OPTION_SWITCH_Y2+OPTION_SWITCH_Y_SPACING*i,
								m.roman_number(i+1) );
						}

						// ----- display start, cancel button ------//

						font_thin_black.center_put( BUTTON2_X1, BUTTON2_Y1, BUTTON2_X2, BUTTON2_Y2,
							"Start" );
						font_thin_black.center_put( BUTTON4_X1, BUTTON4_Y1, BUTTON4_X2, BUTTON4_Y2,
							"Cancel" );

						vga.use_front();
						vga.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
					}
					if( refreshFlag & SGOPTION_FOG )
						fogGroup.paint(tempConfig.fog_of_war);

					if( refreshFlag & SGOPTION_SPY_METHOD )
						spyMethodGroup.paint(tempConfig.spy_methodology);

					if( refreshFlag & SGOPTION_RANDOM_EVENT )
						randomEventGroup.paint(tempConfig.random_event_frequency);
				}
*/
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

			if( optionMode == OPTION_SPECIES )
			{
				if( speciesGroup.detect() >= 0 
					|| speciesGroup[speciesGroup()].detect(0,0,0,1) )	// detect pushed button, but suspend pop
				{
					if( speciesGroup() == 0 && tempConfig.race_id < 0 )
						tempConfig.race_id = 1;		// change to the first human race
					else if( speciesGroup() == 1 && tempConfig.race_id >= 0 )
						tempConfig.race_id = -1;	// change to the first monster race

				//	optionMode = OPTION_BASIC;		// auto change to basic mode
					refreshFlag = SGOPTION_ALL;
				}
				else if( mouse.single_click( SCROLL_SHEET_X1, SCROLL_SHEET_Y1, (SCROLL_SHEET_X1+SCROLL_SHEET_X2)/2, SCROLL_SHEET_Y2) )
				{
					if( tempConfig.race_id < 0 )
						tempConfig.race_id = 1;		// change to the first human race
				//	optionMode = OPTION_BASIC;		// auto change to basic mode
					refreshFlag = SGOPTION_ALL;
				}
				else if( mouse.single_click( (SCROLL_SHEET_X1+SCROLL_SHEET_X2)/2+1, SCROLL_SHEET_Y1, SCROLL_SHEET_X2, SCROLL_SHEET_Y2) )
				{
					if( tempConfig.race_id >= 0 )
						tempConfig.race_id = -1;	// change to the first monster race

				//	optionMode = OPTION_BASIC;		// auto change to basic mode
					refreshFlag = SGOPTION_ALL;
				}
			}
			else
			if( optionMode == OPTION_LEARNING_CAMPAIGN )
			{
				if( learningCampaignGroup.detect() >= 0 
					|| learningCampaignGroup[learningCampaignGroup()].detect(0,0,0,1) )	// detect pushed button, but suspend pop
				{
				//	optionMode = OPTION_SPECIES;		// auto change to basic mode
					refreshFlag = SGOPTION_ALL;
				}
			}

		/*	else if( optionMode == OPTION_BASIC )
			{
			//	if( raceGroup.detect() >= 0)
			//	{
			//		tempConfig.race_id = raceGroup[raceGroup()].custom_para.value;
			//		//refreshFlag |= SGOPTION_RACE;
			//	}
				
				if( tempConfig.race_id >= 0 && raceGroup.detect() >= 0)
				{
					tempConfig.race_id = raceGroup[raceGroup()].custom_para.value;
					//refreshFlag |= SGOPTION_RACE;
				}
				else if( tempConfig.race_id < 0 && monsterRaceGroup.detect() >= 0 )
				{
					tempConfig.race_id = monsterRaceGroup[monsterRaceGroup()].custom_para.value;
					//refreshFlag |= SGOPTION_RACE;
				}
				else if( colorGroup.detect() >= 0)
				{
					tempConfig.player_nation_color = colorGroup[colorGroup()].custom_para.value;
					refreshFlag |= SGOPTION_COLOR;
				}
				else if( learningCampDiffGroup.detect() >= 0)
				{
					tempConfig.campaign_difficulty = learningCampDiffGroup[learningCampDiffGroup()].custom_para.value;
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
			}*/

/*			else if( optionMode == OPTION_ADVANCED )
			{
				if( fogGroup.detect() >= 0 )
				{
					tempConfig.fog_of_war = fogGroup[fogGroup()].custom_para.value;
					// refreshFlag |= SGOPTION_FOG;
				}

				else if( spyMethodGroup.detect() >= 0 )
				{
					tempConfig.spy_methodology = spyMethodGroup[spyMethodGroup()].custom_para.value;
					// refreshFlag |= SGOPTION_SPY_METHOD;
				}
				else
				if( randomEventGroup.detect() >= 0)
				{
					tempConfig.random_event_frequency = randomEventGroup[randomEventGroup()].custom_para.value;
					refreshFlag |= SGOPTION_RANDOM_EVENT;
				}
			}
*/
			// -------- detect switch option button ---------//

			for( i = OPTION_LEARNING_CAMPAIGN; i <= OPTION_SPECIES; ++i )
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
				retFlag = learningCampaignGroup() + 1;
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
// ------- end of function Game::select_learning_campaign_menu -----//



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
