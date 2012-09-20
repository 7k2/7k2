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

//Filename    : OGAMSING.CPP
//Description : Single player game interface

#include <cstdlib>
#include <ogame.h>
#include <obitmap.h>
#include <ovga.h>
#include <vga_util.h>
#include <omodeid.h>
#include <ocoltbl.h>
#include <ovgalock.h>
#include <osys.h>
#include <omouse.h>
#include <ofont.h>
#include <oimgres.h>
#include <oinfo.h>
#include <obutton.h>
#include <obutt3d.h> 
#include <ogammenu.h>
#include <obattle.h>
#include <ogfile.h>
#include <oconfig.h>
#include <omusic.h>
#include <obuttcus.h>
#include <ocoltbl.h>
#include <ogeta.h>
#include <oprofile.h>
#include <ot_gmenu.h>


//---------- Define option modes --------//

enum
{ 
	OPTION_SPECIES,
	OPTION_BASIC,
	OPTION_ADVANCED,
	OPTION_ADVANCE2,
	OPTION_GOAL,
};

//------- Declare static functions -------//

static int	select_option2();

static void i_disp_text_button(ButtonCustom *, int repaintBody);
static void i_disp_race_button(ButtonCustom *, int repaintBody);
static void i_disp_color_button(ButtonCustom *, int repaintBody);
static void i_disp_int_button(ButtonCustom *, int repaintBody);
static void i_disp_tick_button(ButtonCustom *, int repaintBody);


//-------- Begin of function Game::single_player_game --------//
//
// <int> noAI - if there should be no AI in the game.
//
void Game::single_player_game(int noAI)
{
	if( !select_option2() )
		return;

	// ------ attempt to save the config -------//
	config.save("config.dat");

	//------ start single player game ----------//

	init();
	battle.run(0);			// 0-not multiplayer game
	deinit_all();
}
//--------- End of function Game::single_player_game ---------//


// ------- begin of static function select_option2 -------//
//
// single player menu
//
// return 1 = ok, 0 = cancel
//
static int select_option2()
{
	char optionMode = OPTION_SPECIES;

	const int SHEET_X1 = 95;
	const int SHEET_Y1 = 74;
	const int SHEET_X2 = 703;
	const int SHEET_Y2 = 439;

	Config tempConfig = config;

	TempGameSet tempGameSet(1);		// game_set.open_set
	TempUnitRes tempUnitRes;			// unit_res.init
	TempRaceRes tempRaceRes;			// race_res.init
	TempMonsterRes tempMonsterRes;			// race_res.init

	// last multi-player game may set ai_human_nation_count to zero
	// ##### begin Gilbert 31/12 ######//
	if( tempConfig.ai_nation_count() <= 0 )
	{
		tempConfig.ai_human_nation_count = MAX_NATION/2;
		tempConfig.ai_monster_nation_count = MAX_NATION - 1 - tempConfig.ai_human_nation_count;
	}
	// ##### end Gilbert 31/12 ######//

	// some setting may be modified in the last game
	if( tempConfig.difficulty_level != OPTION_CUSTOM )
		tempConfig.change_difficulty(tempConfig.difficulty_level);

	// ######## begin Gilbert 24/5 ##########//
	// clear goals
	tempConfig.clear_goals();
	// ######## end Gilbert 24/5 ##########//

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

		// ------- palette description -------------//

		PalDescFile palBufDesc( str, 8, 3, 0x100, 8 );

		//-------- create color remap table ---------//
		
		ColorTable colorTable;
		colorTable.generate_table_fast( 0, palBufDesc, ColorTable::bright_func );
		memcpy( colorRemapTable, colorTable.get_table(0), sizeof(colorRemapTable) );
	}

	// -------- initialize info.random_seed field ------//

	const int mapIdSize = 11;		// enough to hold a dword in decimal
	char mapIdStr[mapIdSize+1];
	info.init_random_seed(0);
	ltoa( info.random_seed , mapIdStr, 10);
	// GetA mapIdField;

	// ------ initialize human / fryhtan button ------//

	ButtonCustomGroup speciesGroup(2);
	
	w = font_thin_black.text_width( text_game_menu.str_species_humans() ); //  "Humans" 
	h = font_thin_black.text_height();
	//SXM
	cx = 256;
#if(defined(CHINESE))
	cx = 250;
#endif
	//SXM
	cy = 116;
	speciesGroup[0].create( cx-w/2-10, cy-h/2-5, cx+w/2+10, cy+h/2+5,
		i_disp_text_button, ButtonCustomPara( text_game_menu.str_species_humans(), 0 ), 0 );

	w = font_thin_black.text_width( text_game_menu.str_species_fryhtans() );
	h = font_thin_black.text_height();
	cx = 532;
	cy = 116;
	speciesGroup[1].create( cx-w/2-10, cy-h/2-5, cx+w/2+10, cy+h/2+5,
		i_disp_text_button, ButtonCustomPara( text_game_menu.str_species_fryhtans(), 1 ), 0 );

	// -------- initialize player name field ------//

	GetA playerNameField;
	playerNameField.init( 364, 89, 688, tempConfig.player_name,
		tempConfig.PLAYER_NAME_LEN, &font_bold_black, 0, 1 );

	// -------- initialize human group ---------//

	ButtonCustomGroup raceGroup(MAX_RACE);

	err_when( !race_res.init_flag );
	cx = 210;
	cy = 160;
	const int raceGroupYSpacing = 22;
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
	const int monsterGroupYSpacing = 22;
	for( i = 0; i < MAX_MONSTER_TYPE; ++i, (cy += monsterGroupYSpacing) )		// MAX_MONSTER
	{
		w = font_thin_black.text_width( monster_res[i+1]->name );
		h = monsterGroupYSpacing;
		monsterRaceGroup[i].create( cx-w/2-5, cy-h/2, cx+w/2+5, cy+h/2,
			i_disp_race_button, ButtonCustomPara( NULL, -(i+1) ), 0 );
	}

	// ------- create color group --------//

	ButtonCustomGroup colorGroup(MAX_COLOR_SCHEME);
	const int colorButtonFrameX = 382;
	const int colorButtonFrameY = 155;
	cx = 391;
	cy = 162;
	const int colorButtonWidth = 29;
	const int colorButtonHeight = 30;
	for(i = 0; i < MAX_COLOR_SCHEME; ++i, (cx+=colorButtonWidth) )
	{
		colorGroup[i].create( cx, cy, cx+colorButtonWidth-1, cy+colorButtonHeight-1, 
			i_disp_color_button, ButtonCustomPara(NULL, i+1), 0 );
	}

	// -------- create ai human nation group -------//

	ButtonCustomGroup aiHumanNationGroup(MAX_NATION);
	cx = 361;
	cy = 258-20;
	const int nationButtonSpacing = 30;
	for( i = 0; i < MAX_NATION; ++i, (cx+=nationButtonSpacing) )
	{
		aiHumanNationGroup[i].create( cx-10, cy-10, cx+10, cy+10,
			i_disp_int_button, ButtonCustomPara(NULL, i), 0 );
	}
	// aiHumanNationGroup[0].visible_flag = 0;		// disable 0 computer player

	// -------- create ai monster nation group -------//

	ButtonCustomGroup aiMonsterNationGroup(MAX_NATION);
	cx = 361;
	cy = 280-20;
	for( i = 0; i < MAX_NATION; ++i, (cx+=nationButtonSpacing) )
	{
		aiMonsterNationGroup[i].create( cx-10, cy-10, cx+10, cy+10,
			i_disp_int_button, ButtonCustomPara(NULL, i), 0 );
	}
	// aiMonsterNationGroup[0].visible_flag = 0;		// disable 0 computer player

	// ---------- initialize difficulty_level button group -------//

	ButtonCustomGroup diffGroup(OPTION_HARDEST-OPTION_EASIEST+2);		// 0-7 + OPTION_CUSTOM
	char diffButtonStr[OPTION_HARDEST-OPTION_EASIEST+1][10];
	cx = 360;
	cy = 331-35;
	for( i = 0; i < OPTION_HARDEST-OPTION_EASIEST+1; ++i )
	{
		strcpy( diffButtonStr[i], misc.roman_number(i+1) );
		w = font_thin_black.text_width(diffButtonStr[i]);
		h = font_thin_black.text_height();
		diffGroup[i].create( cx, cy, cx+w+10, cy+h+10,
			i_disp_text_button, ButtonCustomPara(diffButtonStr[i], i+OPTION_EASIEST), 0);
		cx += w + 12;
	}

	w = font_thin_black.text_width(text_game_menu.str_difficulty_custom());
	h = font_thin_black.text_height();
	diffGroup[i].create( cx, cy, cx+w+10, cy+h+10,
		i_disp_text_button, ButtonCustomPara(text_game_menu.str_difficulty_custom(), OPTION_CUSTOM), 0);
	cx += w + 12;

	// --------- initialize terrain group --------//

	ButtonCustomGroup terrainGroup(2);
	// char *terrainButtonStr[3] = { "Fertile", "Badlands", "Arctic" };
	cx = 370+45;
	cy = 406-65;
	for( i = 0; i < terrainGroup.button_num; ++i )
	{
		w = font_thin_black.text_width( text_game_menu.str_terrain_set(i+1) );
		h = font_thin_black.text_height();
		terrainGroup[i].create( cx, cy, cx+w+10, cy+h+10, 
			i_disp_text_button, ButtonCustomPara(text_game_menu.str_terrain_set(i+1), i+1), 0 );
		cx += w + 12;
	}

	// --------- initialize building size --------//

	ButtonCustomGroup buildingSizeGroup(2);
	// char *buildingSizeButtonStr[2] = { "Large", "Small" };
	cx = 370+63;
	cy = 386;
	for( i = 0; i < buildingSizeGroup.button_num; ++i )
	{
		w = font_thin_black.text_width( text_game_menu.str_building_set(i+1) );
		h = font_thin_black.text_height();
		buildingSizeGroup[i].create( cx, cy, cx+w+10, cy+h+10, 
			i_disp_text_button, ButtonCustomPara(text_game_menu.str_building_set(i+1), i+1), 0 );
		cx += w + 12;
	}
	
	// ------- initialize explored group ------//

	const int option3X = 320;
	ButtonCustomGroup exploreGroup(2);
	cx = option3X;
	cy = 99;
	w = font_thin_black.text_width( text_game_menu.str_world_map(0) );	// "Unexplored"
	h = font_thin_black.text_height();
	exploreGroup[0].create(cx, cy, cx+w+10, cy+h+10,
		i_disp_text_button, ButtonCustomPara( text_game_menu.str_world_map(0), 0), 0 );
	cx += w+20;
	w = font_thin_black.text_width( text_game_menu.str_world_map(1) );	// "Explored"
	exploreGroup[1].create(cx, cy, cx+w+10, cy+h+10,
		i_disp_text_button, ButtonCustomPara( text_game_menu.str_world_map(1), 1), 0 );

	// ------ initialize fog of war ------//

	ButtonCustomGroup fogGroup(2);
	cx = option3X;
	cy = 143;
	w = font_thin_black.text_width( text_game_menu.str_off_on(0) );	// "Off"
	h = font_thin_black.text_height();
	fogGroup[0].create( cx, cy, cx+w+10, cy+h+10,
		i_disp_text_button, ButtonCustomPara( text_game_menu.str_off_on(0), 0), 0 );
	cx += w+20;
	w = font_thin_black.text_width( text_game_menu.str_off_on(1) );
	fogGroup[1].create( cx, cy, cx+w+10, cy+h+10,
		i_disp_text_button, ButtonCustomPara( text_game_menu.str_off_on(1), 1), 0 );

	// --------- initialize start_up_cash/start_up_food button group -------//

//	char *treasureSizeStr[4] = { "Small", "Medium", "Large", "Huge" };

	ButtonCustomGroup treasureGroup(4);
	cx = option3X;
	cy = 183;
	for( i = 0; i < 4; ++i )
	{
		w = font_thin_black.text_width( text_game_menu.str_treasure(i+1) );
		h = font_thin_black.text_height();
		treasureGroup[i].create(cx, cy, cx+w+10, cy+h+10,
			i_disp_text_button, ButtonCustomPara( text_game_menu.str_treasure(i+1), i+1), 0 );
		cx += w+20;
	}

	// --------- initialize ai_start_up_cash/food button group -------//

	ButtonCustomGroup aiTreasureGroup(4);
	cx = option3X;
	cy = 235;
	for( i = 0; i < 4; ++i )
	{
		w = font_thin_black.text_width( text_game_menu.str_treasure(i+1) );
		h = font_thin_black.text_height();
		aiTreasureGroup[i].create(cx, cy, cx+w+10, cy+h+10,
			i_disp_text_button, ButtonCustomPara( text_game_menu.str_treasure(i+1), i+1), 0 );
		cx += w+20;
	}

	// --------- initialize ai_aggressiveness -------//

	// #### begin Gilbert 10/2 #########//
//	char *aggressiveStr[5] = { "Very Low", "Low", "Medium", "Great", "Very great" };

	ButtonCustomGroup aiAggressiveGroup(5);
	cx = option3X;
	cy = 289;
	for( i = 0; i < 5; ++i )
	{
		w = font_thin_black.text_width( text_game_menu.str_aggressiveness(i) );
		h = font_thin_black.text_height();
		aiAggressiveGroup[i].create(cx, cy, cx+w+10, cy+h+10,
			i_disp_text_button, ButtonCustomPara( text_game_menu.str_aggressiveness(i), i), 0 );
		if( aiAggressiveGroup[i].x2 > SCROLL_SHEET_X2 )
		{	// line feed
			cy = aiAggressiveGroup[i].y2+1 ;
			cx = option3X;
			aiAggressiveGroup[i].create(cx, cy, cx+w+10, cy+h+10,
				i_disp_text_button, ButtonCustomPara( text_game_menu.str_aggressiveness(i), i), 0 );
		}
		cx += w+24;
	}
	// #### end Gilbert 10/2 #########//

	// --------- initialize spy methodology-------//

	// char *spyMethodStr[2] = { "Must  Research", "All  Available" };

	ButtonCustomGroup spyMethodGroup(2);
	cx = option3X;
	cy = 348;
	for( i = 0; i < 2; ++i )
	{
		w = font_thin_black.text_width( text_game_menu.str_need_research(i) );
		h = font_thin_black.text_height();
		spyMethodGroup[i].create( cx, cy, cx+w+10, cy+h+5,
			i_disp_text_button, ButtonCustomPara( text_game_menu.str_need_research(i), i), 0 );
		cx += w+24;
	}

	// --------- initialize random startup button group -------//

//	char *yesNoStr[4] = { "No", "Yes" };

	ButtonCustomGroup randomStartUpGroup(2);
	cx = option3X;
	cy = 389;
	for( i = 0; i < 2; ++i )
	{
		w = font_thin_black.text_width( text_game_menu.str_no_yes(i) );
		h = font_thin_black.text_height();
		randomStartUpGroup[i].create(cx, cy, cx+w+10, cy+h+10,
			i_disp_text_button, ButtonCustomPara(text_game_menu.str_no_yes(i), i), 0);
		cx += w+20;
	}

	//  -------- initialize start_up_raw_site buttons --------- //

	const int option4X = 410;
	ButtonCustomGroup rawSiteGroup(7);
	cx = option4X;
	cy = 100;
	for( i = 0; i < 7; ++i )
	{
		rawSiteGroup[i].create(cx, cy, cx+15, cy+font_thin_black.height()+10,
			i_disp_int_button, ButtonCustomPara(NULL, i+1), 0);
		cx += 25;
	}

	// --------- initialize start_up_has_mine_nearby button group --------//

	ButtonCustomGroup nearRawGroup(2);
	cx = option4X;
	cy = 152;
	for( i = 0; i < 2; ++i )
	{
		w = font_thin_black.text_width( text_game_menu.str_no_yes(i) );
		h = font_thin_black.text_height();
		nearRawGroup[i].create(cx, cy, cx+w+10, cy+h+10,
			i_disp_text_button, ButtonCustomPara(text_game_menu.str_no_yes(i), i), 0);
		cx += w+20;
	}

	// --------- initialize start_up_independent_town button group --------//

	short startTownArray[3] = { 7, 15, 30 };
//	char *fewManyStr[3] = { "Few", "More", "Many" };

	ButtonCustomGroup townStartGroup(3);
	cx = option4X;
	cy = 206;
	for( i = 0; i < 3; ++i )
	{
		w = font_thin_black.text_width( text_game_menu.str_few_to_many(i+1) );
		h = font_thin_black.text_height();
		townStartGroup[i].create(cx, cy, cx+w+10, cy+h+10,
			i_disp_text_button, ButtonCustomPara(text_game_menu.str_few_to_many(i+1), startTownArray[i]), 0);
		cx += w+20;
	}

	// --------- initialize independent_town_resistance button group --------//

//	char *lowHighStr[3] = { "Low", "Med", "High" };

	ButtonCustomGroup townResistGroup(3);
	cx = option4X;
	cy = 246;
	for( i = 0; i < 3; ++i )
	{
		w = font_thin_black.text_width( text_game_menu.str_low_to_high(i+1) );
		h = font_thin_black.text_height();
		townResistGroup[i].create(cx, cy, cx+w+10, cy+h+10,
			i_disp_text_button, ButtonCustomPara(text_game_menu.str_low_to_high(i+1), i+1), 0);
		cx += w+20;
	}

	// --------- initialize new_independent_town_emerge button group --------//

	ButtonCustomGroup townEmergeGroup(2);
	cx = option4X;
	cy = 293;
	for( i = 0; i < 2; ++i )
	{
		w = font_thin_black.text_width( text_game_menu.str_no_yes(i) );
		h = font_thin_black.text_height();
		townEmergeGroup[i].create(cx, cy, cx+w+10, cy+h+10,
			i_disp_text_button, ButtonCustomPara(text_game_menu.str_no_yes(i), i), 0 );
		cx += w+20;
	}

	// --------- initialize new_nation_emerge button group --------//

	ButtonCustomGroup nationEmergeGroup(2);
	cx = option4X;
	cy = 340;
	for( i = 0; i < 2; ++i )
	{
		w = font_thin_black.text_width( text_game_menu.str_no_yes(i) );
		h = font_thin_black.text_height();
		nationEmergeGroup[i].create(cx, cy, cx+w+10, cy+h+10,
			i_disp_text_button, ButtonCustomPara(text_game_menu.str_no_yes(i), i), 0 );
		cx += w+20;
	}

	// --------- initialize random_event_frequency button group --------//

//	char *randomEventStr[4] = { "Never", "Seldom", "Occasional", "Frequent" };

	ButtonCustomGroup randomEventGroup(4);
	cx = option4X;
	cy = 384;
	for( i = 0; i < 4; ++i )
	{
		w = font_thin_black.text_width( text_game_menu.str_never_to_frequent(i) );
		h = font_thin_black.text_height();
		randomEventGroup[i].create(cx, cy, cx+w+10, cy+h+6,
			i_disp_text_button, ButtonCustomPara(text_game_menu.str_never_to_frequent(i), i), 0);
		if( (i+1) % 2 == 0 )
		{
			// line feed
			cx = option4X;
			cy += 26;
		}
		else
		{
			cx += 144;
		}
	}

	// ------- initialize goal button ---------- //

	// ###### begin Gilbert 11/3 #########//
	ButtonCustom clearEnemyButton, clearMonsterButton, allyWinButton, enoughPeopleButton, enoughIncomeButton, enoughScoreButton, timeLimitButton;
	Button peopleInc, peopleDec, incomeInc, incomeDec, scoreInc, scoreDec, yearInc, yearDec;

	const int option5X = 126;
	const int tickWidth = 20;
	const int tickHeight = 20;
	const int option5X2 = 620;
	const int option5X3 = 642;
	w = tickWidth;
	h = tickHeight;

	clearEnemyButton.create( option5X, 145, option5X+w-1, 145+h-1,
		i_disp_tick_button, ButtonCustomPara(NULL, 0), 0, 1);
	clearEnemyButton.enable_flag = 0;
	clearMonsterButton.create( option5X, 178, option5X+w-1, 178+h-1,
		i_disp_tick_button, ButtonCustomPara(NULL, 0), 0, 
		tempConfig.goal_destroy_monster);
	allyWinButton.create( option5X, 211, option5X+w-1, 211+h-1,
		i_disp_tick_button, ButtonCustomPara(NULL, 0), 0, 
		tempConfig.goal_alliance_win_flag);
	enoughPeopleButton.create( option5X, 244, option5X+w-1, 244+h-1,
		i_disp_tick_button, ButtonCustomPara(NULL, 0), 0, 
		tempConfig.goal_population_flag);
	enoughIncomeButton.create( option5X, 277, option5X+w-1, 277+h-1,
		i_disp_tick_button, ButtonCustomPara(NULL, 0), 0,
		tempConfig.goal_economic_score_flag);
	enoughScoreButton.create( option5X, 310, option5X+w-1, 310+h-1,
		i_disp_tick_button, ButtonCustomPara(NULL, 0), 0,
		tempConfig.goal_total_score_flag);
	timeLimitButton.create( option5X, 343, option5X+w-1, 343+h-1,
		i_disp_tick_button, ButtonCustomPara(NULL, 0), 0,
		tempConfig.goal_year_limit_flag);

	peopleInc.create_text( option5X2, 244, option5X2+w-1, 244+h-1, "+" );
	peopleDec.create_text( option5X3, 244, option5X3+w-1, 244+h-1, "-" );

	incomeInc.create_text( option5X2, 277, option5X2+w-1, 277+h-1, "+" );
	incomeDec.create_text( option5X3, 277, option5X3+w-1, 277+h-1, "-" );

	scoreInc.create_text( option5X2, 310, option5X2+w-1, 310+h-1, "+" );
	scoreDec.create_text( option5X3, 310, option5X3+w-1, 310+h-1, "-" );

	yearInc.create_text( option5X2, 343, option5X2+w-1, 343+h-1, "+" );
	yearDec.create_text( option5X3, 343, option5X3+w-1, 343+h-1, "-" );
	// ###### end Gilbert 11/3 #########//

	// ------- loop ---------//

	{
		VgaFrontLock vgaLock;

		while(1)
		{
#ifndef NO_WINDOWS  // FIXME
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
#endif
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
						vga_util.disp_image_file("CHOOSE");

						// ------- display option Mode ------//

						for( i = OPTION_SPECIES; i <= OPTION_GOAL; ++i )
						{
							// red font for selected
							Font *fontPtr = (i == optionMode ? &font_bold_red : &font_bold_black );

							fontPtr->center_put(OPTION_SWITCH_X1, OPTION_SWITCH_Y1+OPTION_SWITCH_Y_SPACING*i,
								OPTION_SWITCH_X2, OPTION_SWITCH_Y2+OPTION_SWITCH_Y_SPACING*i, 
								misc.roman_number(i+1) );
						}

						// ----- display picture -------//

						const int pictureWidth = 209; // 298;
						const int pictureHeight = 210; // 300;
						const int pictureXoffset = 35;
						const int pictureYoffset = 20;

						if( !vga.use_back_buf )
						{
							image_interface.put_front( (SCROLL_SHEET_X1+SCROLL_SHEET_X2)/2-pictureWidth-pictureXoffset, 
								SCROLL_SHEET_Y2-300+pictureYoffset, "HUMANS" );
							image_interface.put_front( (SCROLL_SHEET_X1+SCROLL_SHEET_X2)/2+pictureXoffset, 
								SCROLL_SHEET_Y2-300+pictureYoffset, "FRYHTANS" );
						}
						else
						{
							image_interface.put_back( (SCROLL_SHEET_X1+SCROLL_SHEET_X2)/2-pictureWidth-pictureXoffset, 
								SCROLL_SHEET_Y2-300+pictureYoffset, "HUMANS" );
							image_interface.put_back( (SCROLL_SHEET_X1+SCROLL_SHEET_X2)/2+pictureXoffset, 
								SCROLL_SHEET_Y2-300+pictureYoffset, "FRYHTANS" );
						}

						// ----- display start, cancel button ------//

						font_thin_black.center_put( BUTTON2_X1, BUTTON2_Y1, BUTTON2_X2, BUTTON2_Y2,
							text_game_menu.str_start() );
						font_thin_black.center_put( BUTTON4_X1, BUTTON4_Y1, BUTTON4_X2, BUTTON4_Y2,
							text_game_menu.str_cancel() );

						vga.use_front();
						vga_util.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
					}
					if( refreshFlag & SGOPTION_RACE )
						speciesGroup.paint( tempConfig.race_id < 0 );

					// other ...
				}

				// ------- display basic option ---------//
				if( optionMode == OPTION_BASIC )
				{
					if( refreshFlag & SGOPTION_PAGE )
					{
						vga.use_back();
						vga_util.disp_image_file("CHOOSE");
						// BUGHERE : option menu column and finger

						font_bold_black.right_put( playerNameField.x, playerNameField.y,
							text_game_menu.str_kingdom_of()); // "Kingdom of " );

						if( tempConfig.race_id > 0 )
							font_bold_black.center_put( 116, 126, 303, 146,	text_game_menu.str_nationality()); // "Nationality");
						else
							font_bold_black.center_put( 116, 126, 303, 146,	text_game_menu.str_species()); // "Fryhtan Species");
						font_bold_black.center_put( 382, 129, 600, 149, text_game_menu.str_color()); // "Color" );
						font_bold_black.center_put( 297, 225-20, 698, 243-20, text_game_menu.str_ai_nation_count()); // "Computer Controlled Kingdoms" );
						font_bold_black.put( aiHumanNationGroup[MAX_NATION-1].x2+10, aiHumanNationGroup[MAX_NATION-1].y1,
							text_game_menu.str_human() ); // "Human" );
						font_bold_black.put( aiMonsterNationGroup[MAX_NATION-1].x2+10, aiMonsterNationGroup[MAX_NATION-1].y1,
							text_game_menu.str_fryhtan() ); //"Fryhtan" );
						font_bold_black.center_put( 341, 305-30, 654, 324-30, text_game_menu.str_difficulty_level()); // "Difficulty Level" );
						font_bold_black.center_put( 370, 380-60, 633, 393-60, text_game_menu.str_terrain_set()); // "Terrain Type" );
						font_bold_black.center_put( 341, 365, 660, 384, text_game_menu.str_building_set()); // "Building Size" );

						// ------- display option Mode ------//

						for( i = OPTION_SPECIES; i <= OPTION_GOAL; ++i )
						{
							// red font for selected
							Font *fontPtr = (i == optionMode ? &font_bold_red : &font_bold_black );

							fontPtr->center_put(OPTION_SWITCH_X1, OPTION_SWITCH_Y1+OPTION_SWITCH_Y_SPACING*i,
								OPTION_SWITCH_X2, OPTION_SWITCH_Y2+OPTION_SWITCH_Y_SPACING*i, 
								misc.roman_number(i+1) );
						}

						// ----- display start, cancel button ------//

						font_thin_black.center_put( BUTTON2_X1, BUTTON2_Y1, BUTTON2_X2, BUTTON2_Y2,
							text_game_menu.str_start() );
						font_thin_black.center_put( BUTTON4_X1, BUTTON4_Y1, BUTTON4_X2, BUTTON4_Y2,
							text_game_menu.str_cancel() );

						vga.use_front();
						vga_util.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
					}

					if( refreshFlag & SGOPTION_RACE )
					{
						if( tempConfig.race_id >= 0 )
							raceGroup.paint( tempConfig.race_id-1 );
						else
						{
							monsterRaceGroup.paint( -tempConfig.race_id-1 );
							if (tempConfig.race_id < 0)
							{
								char str[10] = "P-X";
								str[2] = '0' - tempConfig.race_id;
								char* bitmapPtr = image_interface.get_ptr(str);
								// all p-x must be the same size
								// center to 220,370
								const int picWidth = 198;
								const int picHeight = 121;
								const int picX = 220-picWidth/2;
								const int picY = 370-picHeight/2;
								if( !vga.use_back_buf )
								{
									mouse.hide_area(picX, picY, picX+picWidth-1, picY+picHeight-1 );
									vga.active_buf->join_trans( &vga_back, picX, picY, bitmapPtr );
									mouse.show_area();
								}
								else
								{
									vga.active_buf->put_bitmap_trans( picX, picY, bitmapPtr );
								}
							}
							else
							{
								// actually, need to clean background here
							}
						}
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

						vga_util.blt_buf( colorButtonFrameX, colorButtonFrameY,
							colorButtonFrameX + ((Bitmap *)bitmapPtr)->get_width() - 1,
							colorButtonFrameY + ((Bitmap *)bitmapPtr)->get_height() - 1, 0 );
					}
					if( refreshFlag & SGOPTION_AI_NATION )
					{
						aiHumanNationGroup.paint(tempConfig.ai_human_nation_count);
						aiMonsterNationGroup.paint(tempConfig.ai_monster_nation_count);
					}
					if( refreshFlag & SGOPTION_DIFFICULTY )
					{
						if( tempConfig.difficulty_level != OPTION_CUSTOM )
							diffGroup.paint(tempConfig.difficulty_level);
						else
							diffGroup.paint(diffGroup.button_num-1);	// last button
					}
					if( refreshFlag & SGOPTION_TERRAIN )
						terrainGroup.paint(tempConfig.terrain_set-1);
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
						vga_util.disp_image_file("CHOOSE");

						font_bold_black.put_paragraph( 126, 99, option3X-10, 143-1,
							text_game_menu.str_world_map()); // "World Map" );
						font_bold_black.put_paragraph( 126, 143, option3X-10, 183-1,
							text_game_menu.str_fog_of_war()); // "Fog of War" );
						font_bold_black.put_paragraph( 126, 183, option3X-10, 235-1,
						//	"Human Player's Treasure" );
							text_game_menu.str_your_treasure()); // "Your Treasure" );
						font_bold_black.put_paragraph( 126, 235, option3X-10, 289-1,
							text_game_menu.str_ai_treasure()); // "Computer's Treasure" );
						font_bold_black.put_paragraph( 126, 289, option3X-10, 339-1,
							text_game_menu.str_ai_aggressiveness()); // "Computer's Aggressiveness" );
#if(defined(GERMAN))
						font_bold_black.put_paragraph( 126, 348, option3X-10, 389-1,
							text_game_menu.str_spy_methodology()); // "Espionage Methodologies" );
#else
						font_bold_black.put_paragraph( 126, 339, option3X-10, 389-1,
							text_game_menu.str_spy_methodology()); // "Espionage Methodologies" );
#endif
						font_bold_black.put_paragraph( 126, 389, option3X-10, 449-1,
							text_game_menu.str_random_kingdom()); // "Random Kingdoms" );

						// ------- display option Mode ------//

						for( i = OPTION_SPECIES; i <= OPTION_GOAL; ++i )
						{
							// red font for selected
							Font *fontPtr = (i == optionMode ? &font_bold_red : &font_bold_black );

							fontPtr->center_put(OPTION_SWITCH_X1, OPTION_SWITCH_Y1+OPTION_SWITCH_Y_SPACING*i,
								OPTION_SWITCH_X2, OPTION_SWITCH_Y2+OPTION_SWITCH_Y_SPACING*i, 
								misc.roman_number(i+1) );
						}

						// ----- display start, cancel button ------//

						font_thin_black.center_put( BUTTON2_X1, BUTTON2_Y1, BUTTON2_X2, BUTTON2_Y2,
							text_game_menu.str_start() );
						font_thin_black.center_put( BUTTON4_X1, BUTTON4_Y1, BUTTON4_X2, BUTTON4_Y2,
							text_game_menu.str_cancel() );

						vga.use_front();
						vga_util.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
					}
					if( refreshFlag & SGOPTION_EXPLORED )
						exploreGroup.paint(tempConfig.explore_whole_map);
					if( refreshFlag & SGOPTION_FOG )
						fogGroup.paint(tempConfig.fog_of_war);
					if( refreshFlag & SGOPTION_TREASURE )
						treasureGroup.paint( tempConfig.start_up_cash-1 );
					if( refreshFlag & SGOPTION_AI_TREASURE )
						aiTreasureGroup.paint( tempConfig.ai_start_up_cash-1 );
					// ####### begin Gilbert 10/2 ########//
					if( refreshFlag & SGOPTION_AI_AGGRESSIVE )
						aiAggressiveGroup.paint(tempConfig.ai_aggressiveness);
					// ####### end Gilbert 10/2 ########//
					if( refreshFlag & SGOPTION_SPY_METHOD )
						spyMethodGroup.paint(tempConfig.spy_methodology);
					if( refreshFlag & SGOPTION_RANDOM_STARTUP )
						randomStartUpGroup.paint(tempConfig.random_start_up);
				}

				// ------- display advanced option ---------//
				if( optionMode == OPTION_ADVANCE2 )
				{
					if( refreshFlag & SGOPTION_PAGE )
					{
						vga.use_back();
						vga_util.disp_image_file("CHOOSE");

						font_bold_black.put_paragraph( 126, 100, option4X-10, 152-1,
							text_game_menu.str_start_up_raw_site()); // "Natural Resources at Start" );
						font_bold_black.put_paragraph( 126, 152, option4X-10, 206-1,
							text_game_menu.str_raw_nearby()); // "Natural Resources Nearby" );
						font_bold_black.put_paragraph( 126, 206, option4X-10, 246-1,
							text_game_menu.str_independent_town()); // "Independent Towns" );
						font_bold_black.put_paragraph( 126, 246, option4X-10, 293-1,
							text_game_menu.str_town_resistance()); // "Independent Town Resistance" );
						font_bold_black.put_paragraph( 126, 293, option4X-10, 340-1,
							text_game_menu.str_new_town_emerge()); // "New Towns Emerge" );
						font_bold_black.put_paragraph( 126, 340, option4X-10, 384-1,
							text_game_menu.str_new_kingdom_emerge()); // "New Kingdoms Emerge" );
						font_bold_black.put_paragraph( 126, 384, option4X-10, 434-1,
							text_game_menu.str_random_events()); // "Random Events" );

						// ------- display option Mode ------//

						for( i = OPTION_SPECIES; i <= OPTION_GOAL; ++i )
						{
							// red font for selected
							Font *fontPtr = (i == optionMode ? &font_bold_red : &font_bold_black );

							fontPtr->center_put(OPTION_SWITCH_X1, OPTION_SWITCH_Y1+OPTION_SWITCH_Y_SPACING*i,
								OPTION_SWITCH_X2, OPTION_SWITCH_Y2+OPTION_SWITCH_Y_SPACING*i, 
								misc.roman_number(i+1) );
						}

						// ----- display start, cancel button ------//

						font_thin_black.center_put( BUTTON2_X1, BUTTON2_Y1, BUTTON2_X2, BUTTON2_Y2,
							text_game_menu.str_start() );
						font_thin_black.center_put( BUTTON4_X1, BUTTON4_Y1, BUTTON4_X2, BUTTON4_Y2,
							text_game_menu.str_cancel() );

						vga.use_front();
						vga_util.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
					}
					if( refreshFlag & SGOPTION_RAW )
						rawSiteGroup.paint(tempConfig.start_up_raw_site-1);
					if( refreshFlag & SGOPTION_NEAR_RAW )
						nearRawGroup.paint(tempConfig.start_up_has_mine_nearby);
					if( refreshFlag & SGOPTION_START_TOWN )
					{
						for( i = 0; tempConfig.start_up_independent_town > startTownArray[i]; ++i );
						townStartGroup.paint(i);
					}
					if( refreshFlag & SGOPTION_TOWN_STRENGTH )
						townResistGroup.paint(tempConfig.independent_town_resistance-1);
					if( refreshFlag & SGOPTION_TOWN_EMERGE )
						townEmergeGroup.paint(tempConfig.new_independent_town_emerge);
					if( refreshFlag & SGOPTION_KINGDOM_EMERGE )
						nationEmergeGroup.paint(tempConfig.new_nation_emerge);
					if( refreshFlag & SGOPTION_RANDOM_EVENT )
						randomEventGroup.paint(tempConfig.random_event_frequency);
				}

				// ------- display goal option ---------//
				if( optionMode == OPTION_GOAL )
				{
					if( refreshFlag & SGOPTION_PAGE )
					{
						vga.use_back();
						vga_util.disp_image_file("CHOOSE");

						font_bold_black.put( option5X, 112, text_game_menu.str_goal() );	// You will be victorious when you have:

						font_thin_black.put_paragraph( option5X+tickWidth+10, 145, option5X3+25, 178-1, 
							text_game_menu.str_defeat_others()); // "Defeated All Others" );
						font_thin_black.put_paragraph( option5X+tickWidth+10, 178, option5X3+25, 211-1, 
							text_game_menu.str_defeat_fryhtan_lairs()); // "Destroyed All Independent Fryhtan Lairs" );
						font_thin_black.put_paragraph( option5X+tickWidth+10, 211, option5X3+25, 244-1, 
							text_game_menu.str_defeat_except_ally()); // "Defeated All except one Allied Kingdom" );
						font_thin_black.put_paragraph( option5X+tickWidth+10, 244, option5X2-65, 277-1, 
							text_game_menu.str_reach_population()); // "Achieved A Population Of" );
						font_thin_black.put_paragraph( option5X+tickWidth+10, 277, option5X2-65, 310-1, 
							text_game_menu.str_reach_economy()); // "Reached An Economic Score Of");
						font_thin_black.put_paragraph( option5X+tickWidth+10, 310, option5X2-65, 343-1, 
							text_game_menu.str_reach_total_score()); //"Reached A Total Score Of");
						font_thin_black.put_paragraph( option5X+tickWidth+10, 343, option5X2-100, 376-1, 
							text_game_menu.str_goal_time_limit()); // "Achieved The Selected Within" );

						// ------- display option Mode ------//

						for( i = OPTION_SPECIES; i <= OPTION_GOAL; ++i )
						{
							// red font for selected
							Font *fontPtr = (i == optionMode ? &font_bold_red : &font_bold_black );

							fontPtr->center_put(OPTION_SWITCH_X1, OPTION_SWITCH_Y1+OPTION_SWITCH_Y_SPACING*i,
								OPTION_SWITCH_X2, OPTION_SWITCH_Y2+OPTION_SWITCH_Y_SPACING*i, 
								misc.roman_number(i+1) );
						}

						// ----- display start, cancel button ------//

						font_thin_black.center_put( BUTTON2_X1, BUTTON2_Y1, BUTTON2_X2, BUTTON2_Y2,
							text_game_menu.str_start() );
						font_thin_black.center_put( BUTTON4_X1, BUTTON4_Y1, BUTTON4_X2, BUTTON4_Y2,
							text_game_menu.str_cancel() );

						vga.use_front();
						vga_util.blt_buf( 0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, 0 );
					}

					font_bold_black.use_max_height();

					if( refreshFlag & SGOPTION_CLEAR_ENEMY )
						clearEnemyButton.paint();
					if( refreshFlag & SGOPTION_CLEAR_MONSTER )
						clearMonsterButton.paint(tempConfig.goal_destroy_monster);
					if( refreshFlag & SGOPTION_CLEAR_ENEMY )
						allyWinButton.paint(tempConfig.goal_alliance_win_flag);
					if( refreshFlag & SGOPTION_ENOUGH_PEOPLE )
					{
						enoughPeopleButton.paint(tempConfig.goal_population_flag);
						font_bold_black.put( peopleInc.x1-65, peopleInc.y1, 
							misc.format(tempConfig.goal_population), 1, peopleInc.x1-1);
						peopleInc.paint();
						peopleDec.paint();
					}
					if( refreshFlag & SGOPTION_ENOUGH_INCOME )
					{
						enoughIncomeButton.paint(tempConfig.goal_economic_score_flag);
						font_bold_black.put( incomeInc.x1-65, incomeInc.y1, 
							misc.format(tempConfig.goal_economic_score), 1, incomeInc.x1-1 );
						incomeInc.paint();
						incomeDec.paint();
					}
					if( refreshFlag & SGOPTION_ENOUGH_SCORE )
					{
						enoughScoreButton.paint(tempConfig.goal_total_score_flag);
						font_bold_black.put( scoreInc.x1-65, scoreInc.y1, 
							misc.format(tempConfig.goal_total_score), 1, scoreInc.x1-1 );
						scoreInc.paint();
						scoreDec.paint();
					}
					if( refreshFlag & SGOPTION_TIME_LIMIT )
					{
						timeLimitButton.paint(tempConfig.goal_year_limit_flag);
						int x2 = font_bold_black.put( yearInc.x1-100, yearInc.y1, 
							misc.format(tempConfig.goal_year_limit), 1, yearInc.x1-60-1 );
						font_thin_black.put( yearInc.x1-60, yearInc.y1, text_game_menu.str_goal_time_units(), 1 );	// "years"
						yearInc.paint();
						yearDec.paint();
					}

					font_bold_black.use_std_height();
				}

				// ----- display difficulty -------//

				if( optionMode != OPTION_SPECIES && refreshFlag & SGOPTION_DIFFICULTY )
				{
					font_bld.center_put( 40, 85, 82, 105, text_game_menu.str_level_in_score(), 1 );	// "Level"
					font_bold_black.center_put( 40, 105, 82, 125, misc.format(tempConfig.single_player_difficulty(0)), 1 );
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

			// ------ detect species option ---------//

			if( optionMode == OPTION_SPECIES )
			{
				if( speciesGroup.detect() >= 0 
					|| speciesGroup[speciesGroup()].detect(0,0,0,1) )	// detect pushed button, but suspend pop
				{
					if( speciesGroup() == 0 && tempConfig.race_id < 0 )
						tempConfig.race_id = 1;		// change to the first human race
					else if( speciesGroup() == 1 && tempConfig.race_id >= 0 )
						tempConfig.race_id = -1;	// change to the first monster race

					optionMode = OPTION_BASIC;		// auto change to basic mode
					refreshFlag = SGOPTION_ALL;
				}
				else if( mouse.single_click( SCROLL_SHEET_X1, SCROLL_SHEET_Y1, (SCROLL_SHEET_X1+SCROLL_SHEET_X2)/2, SCROLL_SHEET_Y2) )
				{
					if( tempConfig.race_id < 0 )
						tempConfig.race_id = 1;		// change to the first human race
					optionMode = OPTION_BASIC;		// auto change to basic mode
					refreshFlag = SGOPTION_ALL;
				}
				else if( mouse.single_click( (SCROLL_SHEET_X1+SCROLL_SHEET_X2)/2+1, SCROLL_SHEET_Y1, SCROLL_SHEET_X2, SCROLL_SHEET_Y2) )
				{
					if( tempConfig.race_id >= 0 )
						tempConfig.race_id = -1;	// change to the first monster race
					optionMode = OPTION_BASIC;		// auto change to basic mode
					refreshFlag = SGOPTION_ALL;
				}
			}

			// ------ detect basic option --------//

			else if( optionMode == OPTION_BASIC )
			{
				if( tempConfig.race_id >= 0 && raceGroup.detect() >= 0)
				{
					tempConfig.race_id = raceGroup[raceGroup()].custom_para.value;
					refreshFlag |= SGOPTION_RACE;
				}
				else if( tempConfig.race_id < 0 && monsterRaceGroup.detect() >= 0 )
				{
					tempConfig.race_id = monsterRaceGroup[monsterRaceGroup()].custom_para.value;
					refreshFlag |= SGOPTION_RACE;
				}
				else if( colorGroup.detect() >= 0)
				{
					tempConfig.player_nation_color = colorGroup[colorGroup()].custom_para.value;
					refreshFlag |= SGOPTION_COLOR;
				}
				else if( aiHumanNationGroup.detect() >= 0 )
				{
					int newHumanNationCount = aiHumanNationGroup[aiHumanNationGroup()].custom_para.value;
					if( newHumanNationCount > 0 )
					{
						tempConfig.ai_human_nation_count = newHumanNationCount;

						// 1 player + ai human + ai monster <= MAX_NATION
						if( 1 + tempConfig.ai_human_nation_count + tempConfig.ai_monster_nation_count > MAX_NATION )
							tempConfig.ai_monster_nation_count = MAX_NATION - 1 - tempConfig.ai_human_nation_count;
					}
					else
					{
						// at least one ai player (single player only)
						if( tempConfig.ai_monster_nation_count > 0 )
						{
							tempConfig.ai_human_nation_count = newHumanNationCount;
						}
					}
					tempConfig.difficulty_level = OPTION_CUSTOM;
					refreshFlag |= SGOPTION_AI_NATION | SGOPTION_DIFFICULTY;
				}
				else if( aiMonsterNationGroup.detect() >= 0 )
				{
					int newMonsterNationCount = aiMonsterNationGroup[aiMonsterNationGroup()].custom_para.value;
					if( newMonsterNationCount > 0 )
					{
						tempConfig.ai_monster_nation_count = newMonsterNationCount;

						// 1 player + ai human + ai monster <= MAX_NATION
						if( 1 + tempConfig.ai_human_nation_count + tempConfig.ai_monster_nation_count > MAX_NATION )
							tempConfig.ai_human_nation_count = MAX_NATION - 1 - tempConfig.ai_monster_nation_count;
					}
					else
					{
						// at least one ai player (single player only)
						if( tempConfig.ai_human_nation_count > 0 )
						{
							tempConfig.ai_monster_nation_count = newMonsterNationCount;
						}
					}
					tempConfig.difficulty_level = OPTION_CUSTOM;
					refreshFlag |= SGOPTION_AI_NATION | SGOPTION_DIFFICULTY;
				}
				else if( diffGroup.detect() >= 0)
				{
					if( diffGroup[diffGroup()].custom_para.value != OPTION_CUSTOM )
					{
						tempConfig.change_difficulty(diffGroup[diffGroup()].custom_para.value);
						// all but SGOPTION_PAGE;
						refreshFlag |= SGOPTION_ALL & ~SGOPTION_PAGE;
					}
					else
					{
						tempConfig.difficulty_level = OPTION_CUSTOM;
						// refreshFlag |= SGOPTION_DIFFICULTY;
					}
				}
				else if( terrainGroup.detect() >= 0)
				{
					tempConfig.terrain_set = terrainGroup[terrainGroup()].custom_para.value;
					static short latitudeArray[3] = { 45, 20, 70 };
					err_when( tempConfig.terrain_set <= 0 || tempConfig.terrain_set > 3 );
					tempConfig.latitude = latitudeArray[tempConfig.terrain_set-1];
					//refreshFlag |= SGOPTION_TERRAIN;
				}
				else if( buildingSizeGroup.detect() >= 0)
				{
					tempConfig.building_size = buildingSizeGroup[buildingSizeGroup()].custom_para.value;
				}
				else if( playerNameField.detect() )
				{
					refreshFlag |= SGOPTION_NAME_FIELD;
				}
			}

			// -------- detect advanced option ---------//

			else if( optionMode == OPTION_ADVANCED )
			{
				if( exploreGroup.detect() >= 0 )
				{
					tempConfig.explore_whole_map = exploreGroup[exploreGroup()].custom_para.value;
					tempConfig.difficulty_level = OPTION_CUSTOM;
					// refreshFlag |= SGOPTION_EXPLORED;
					refreshFlag |= SGOPTION_DIFFICULTY;
				}
				else if( fogGroup.detect() >= 0 )
				{
					tempConfig.fog_of_war = fogGroup[fogGroup()].custom_para.value;
					tempConfig.difficulty_level = OPTION_CUSTOM;
					// refreshFlag |= SGOPTION_FOG
					refreshFlag |= SGOPTION_DIFFICULTY;
				}
				else if( treasureGroup.detect() >= 0 )
				{
					tempConfig.start_up_cash = treasureGroup[treasureGroup()].custom_para.value;
					tempConfig.difficulty_level = OPTION_CUSTOM;
					// refreshFlag |= SGOPTION_TREASURE;
					refreshFlag |= SGOPTION_DIFFICULTY;
				}
				else if( aiTreasureGroup.detect() >= 0 )
				{
					tempConfig.ai_start_up_cash = aiTreasureGroup[aiTreasureGroup()].custom_para.value;
					tempConfig.difficulty_level = OPTION_CUSTOM;
					// refreshFlag |= SGOPTION_AI_TREASURE;
					refreshFlag |= SGOPTION_DIFFICULTY;
				}
				else if( aiAggressiveGroup.detect() >= 0 )
				{
					tempConfig.ai_aggressiveness = 
						aiAggressiveGroup[aiAggressiveGroup()].custom_para.value;
					tempConfig.difficulty_level = OPTION_CUSTOM;
					// refreshFlag |= SGOPTION_AI_AGGRESSIVE;
					refreshFlag |= SGOPTION_DIFFICULTY;
				}
				else if( spyMethodGroup.detect() >= 0 )
				{
					tempConfig.spy_methodology = spyMethodGroup[spyMethodGroup()].custom_para.value;
					tempConfig.difficulty_level = OPTION_CUSTOM;
					// refreshFlag |= SGOPTION_SPY_METHOD;
					refreshFlag |= SGOPTION_DIFFICULTY;
				}
				else if( randomStartUpGroup.detect() >= 0)
				{
					tempConfig.random_start_up = randomStartUpGroup[randomStartUpGroup()].custom_para.value;
					tempConfig.difficulty_level = OPTION_CUSTOM;
					// refreshFlag |= SGOPTION_RANDOM_STARTUP;
					refreshFlag |= SGOPTION_DIFFICULTY;
				}
			}

			// -------- detect advanced option ---------//

			else if( optionMode == OPTION_ADVANCE2 )
			{
				if( rawSiteGroup.detect() >= 0 )
				{
					tempConfig.start_up_raw_site = rawSiteGroup[rawSiteGroup()].custom_para.value;
					tempConfig.difficulty_level = OPTION_CUSTOM;
					// refreshFlag |= SGOPTION_RAW;
					refreshFlag |= SGOPTION_DIFFICULTY;
				}
				else if( nearRawGroup.detect() >= 0)
				{
					tempConfig.start_up_has_mine_nearby = nearRawGroup[nearRawGroup()].custom_para.value;
					tempConfig.difficulty_level = OPTION_CUSTOM;
					// refreshFlag |= SGOPTION_NEAR_RAW;
					refreshFlag |= SGOPTION_DIFFICULTY;

				}
				else if( townStartGroup.detect() >= 0)
				{
					tempConfig.start_up_independent_town = townStartGroup[townStartGroup()].custom_para.value;
					tempConfig.difficulty_level = OPTION_CUSTOM;
					// resfreshFlag |= SGOPTION_START_TOWN;
					refreshFlag |= SGOPTION_DIFFICULTY;
				}
				else if( townResistGroup.detect() >= 0)
				{
					tempConfig.independent_town_resistance = townResistGroup[townResistGroup()].custom_para.value;
					tempConfig.difficulty_level = OPTION_CUSTOM;
					// resfreshFlag |= SGOPTION_TOWN_RESIST;
					refreshFlag |= SGOPTION_DIFFICULTY;
				}
				else if( townEmergeGroup.detect() >= 0)
				{
					tempConfig.new_independent_town_emerge = townEmergeGroup[townEmergeGroup()].custom_para.value;
					tempConfig.difficulty_level = OPTION_CUSTOM;
					// refreshFlag |= SGOPTION_TOWN_EMERGE;
					refreshFlag |= SGOPTION_DIFFICULTY;
				}
				else if( nationEmergeGroup.detect() >= 0)
				{
					tempConfig.new_nation_emerge = nationEmergeGroup[nationEmergeGroup()].custom_para.value;
					tempConfig.difficulty_level = OPTION_CUSTOM;
					// refreshFlag |= SGOPTION_NATION_EMERGE;
					refreshFlag |= SGOPTION_DIFFICULTY;
				}
				else if( randomEventGroup.detect() >= 0)
				{
					tempConfig.random_event_frequency = randomEventGroup[randomEventGroup()].custom_para.value;
					tempConfig.difficulty_level = OPTION_CUSTOM;
					// refreshFlag |= SGOPTION_RANDOM_EVENT;
					refreshFlag |= SGOPTION_DIFFICULTY;
				}
			}

			// -------- detect goal option ----------//

			else if( optionMode == OPTION_GOAL )
			{
				if( clearEnemyButton.detect() )
				{
				}
				else if( clearMonsterButton.detect() )
				{
					tempConfig.goal_destroy_monster = clearMonsterButton.pushed_flag;
				}
				// ######## begin Gilbert 11/3 #########//
				else if( allyWinButton.detect() )
				{
					tempConfig.goal_alliance_win_flag = allyWinButton.pushed_flag;
				}
				// ######## end Gilbert 11/3 #########//
				else if( enoughPeopleButton.detect() )
				{
					tempConfig.goal_population_flag = enoughPeopleButton.pushed_flag;
				}
				else if( enoughIncomeButton.detect() )
				{
					tempConfig.goal_economic_score_flag = enoughIncomeButton.pushed_flag;
				}
				else if( enoughScoreButton.detect() )
				{
					tempConfig.goal_total_score_flag = enoughScoreButton.pushed_flag;
				}
				else if( timeLimitButton.detect() )
				{
					tempConfig.goal_year_limit_flag = timeLimitButton.pushed_flag;
				}
				else if( peopleInc.detect() )
				{
					tempConfig.goal_population += 100;
					if( tempConfig.goal_population > 5000 )
						tempConfig.goal_population = 5000;
					refreshFlag |= SGOPTION_ENOUGH_PEOPLE;
				}
				else if( peopleDec.detect() )
				{
					tempConfig.goal_population -= 100;
					if( tempConfig.goal_population < 100 )
						tempConfig.goal_population = 100;
					refreshFlag |= SGOPTION_ENOUGH_PEOPLE;
				}
				else if( incomeInc.detect() )
				{
					tempConfig.goal_economic_score += 100;
					if( tempConfig.goal_economic_score > 5000 )
					{
						tempConfig.goal_economic_score = 5000;
					}
					refreshFlag |= SGOPTION_ENOUGH_INCOME;
				}
				else if( incomeDec.detect() )
				{
					tempConfig.goal_economic_score -= 100;
					if( tempConfig.goal_economic_score < 100 )
					{
						tempConfig.goal_economic_score = 100;
					}
					refreshFlag |= SGOPTION_ENOUGH_INCOME;
				}
				else if( scoreInc.detect() )
				{
					if( tempConfig.goal_total_score >= 2000 )
						tempConfig.goal_total_score += 500;
					else
						tempConfig.goal_total_score += 100;
					if( tempConfig.goal_total_score > 10000 )
						tempConfig.goal_total_score = 10000;
					refreshFlag |= SGOPTION_ENOUGH_SCORE;
				}
				else if( scoreDec.detect() )
				{
					if( tempConfig.goal_total_score > 2000 )
						tempConfig.goal_total_score -= 500;
					else
						tempConfig.goal_total_score -= 100;
					if( tempConfig.goal_total_score < 100 )
						tempConfig.goal_total_score = 100;
					refreshFlag |= SGOPTION_ENOUGH_SCORE;
				}
				else if( yearInc.detect() )
				{
					if( tempConfig.goal_year_limit >= 20 )
						tempConfig.goal_year_limit += 5;
					else
						tempConfig.goal_year_limit++;
					if( tempConfig.goal_year_limit > 100 )
					{
						tempConfig.goal_year_limit = 100;
					}
					refreshFlag |= SGOPTION_TIME_LIMIT;
				}
				else if( yearDec.detect() )
				{
					if( tempConfig.goal_year_limit > 20 )
						tempConfig.goal_year_limit -= 5;
					else
						tempConfig.goal_year_limit--;
					if( tempConfig.goal_year_limit < 1 )
					{
						tempConfig.goal_year_limit = 1;
					}
					refreshFlag |= SGOPTION_TIME_LIMIT;
				}
			}

			// -------- detect switch option button ---------//

			for( i = OPTION_SPECIES; i <= OPTION_GOAL; ++i )
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
				retFlag = 1;
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
		info.init_random_seed( atol(mapIdStr) );
		tempConfig.human_difficulty_rating = tempConfig.single_player_difficulty(1);
		tempConfig.monster_difficulty_rating = tempConfig.single_player_difficulty(-1);
		config = tempConfig;
	}

	return retFlag;
}
// ------- end of static function select_option2 -------//


// ------ begin of static function i_disp_text_button ------//
//
// BustomCustom callback function to display text on top centre
//
static void i_disp_text_button(ButtonCustom *button, int repaintBody)
{
	if( !vga.use_back_buf )
		vga_util.blt_buf( button->x1, button->y1, button->x2, button->y2, 0 );
	Font *fontPtr = button->pushed_flag ? &font_bold_red : &font_thin_black;
//	fontPtr->space_width -= 8;	
	// top center align
	fontPtr->center_put( button->x1, button->y1, button->x2, button->y1+fontPtr->font_height-1,
		(char *)button->custom_para.ptr );
//	fontPtr->space_width += 8;	
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
		vga_util.blt_buf( button->x1, button->y1, button->x2, button->y2, 0 );
	Font *fontPtr = button->pushed_flag ? &font_bold_red : &font_thin_black;
	// top center align
	fontPtr->center_put( button->x1, button->y1, button->x2, button->y1+fontPtr->font_height-1,
		raceId >= 0 ? race_res[raceId]->name : monster_res[-raceId]->name );
}
// ------ end of static function i_disp_race_button ------//


// ------ begin of static function i_disp_int_button ------//
//
// BustomCustom callback function to display integer on top centre
//
static void i_disp_int_button(ButtonCustom *button, int repaintBody)
{
	if( !vga.use_back_buf )
		vga_util.blt_buf( button->x1, button->y1, button->x2, button->y2, 0 );
	Font *fontPtr = button->pushed_flag ? &font_bold_red : &font_thin_black;
	// top center align
	fontPtr->center_put( button->x1, button->y1, button->x2, button->y1+fontPtr->font_height-1,
		misc.format(button->custom_para.value) );
}
// ------ end of static function i_disp_int_button ------//


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



// ------ begin of static function i_disp_tick_button ------//
//
// BustomCustom callback function to display a tick
//
static void i_disp_tick_button(ButtonCustom *button, int repaintBody)
{
	if( !button->pushed_flag )
	{
		vga.active_buf->d3_panel_up( button->x1, button->y1, button->x2, button->y2 );
	}
	else
	{
		vga.active_buf->put_bitmap_trans_decompress( button->x1+4, button->y1+4, image_button.read("TICK") );
		vga.active_buf->d3_panel_down( button->x1, button->y1, button->x2, button->y2, 4, 0 );
	}
}
// ------ end of static function i_disp_tick_button ------//
