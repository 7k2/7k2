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

//Filename    : OGAME.CPP
//Description : Main Game Object

#include <OGAME.H>
#include <OGAMESET.H>
#include <ALL.H>
#include <COLCODE.H>
#include <OSYS.H>
#include <OVGA.H>
#include <OMODEID.H>
#include <OFONT.H>
#include <OMOUSE.H>
#include <OMOUSECR.H>
#include <OBUTTON.H>
#include <OCONFIG.H>
#include <OUNITALL.H>
#include <OBULLET.H>
#include <OTERRAIN.H>
#include <OPLANT.H>
#include <OWORLD.H>
#include <OWORLDMT.H>
#include <OPOWER.H>
#include <OSITE.H>
#include <ORACERES.H>
#include <OWALLRES.H>
#include <OTECHRES.H>
#include <OSPRTRES.H>
#include <OSFRMRES.H>
#include <OGODRES.H>
#include <OMONSRES.H>
#include <OHERORES.H>
#include <OWAYPNT.H>
#include <OTOWN.H>
#include <ONATION.H>
#include <OFIRM.H>
#include <OIMGRES.H>
#include <OINFO.H>
#include <OSPRITE.H>
#include <OREBEL.H>
#include <OSPY.H>
#include <OBATTLE.H>
#include <ONEWS.H>
#include <OWEATHER.H>
#include <OEVENT.H>
#include <OHILLRES.H>
#include <OTALKRES.H>
#include <OSNOWRES.H>
#include <OSNOWG.H>
#include <OEXPMASK.H>
#include <OSE.H>
#include <OSERES.H>
#include	<OROCKRES.H>
#include	<OROCK.H>
#include <OEFFECT.H>
#include <OAUDIO.H>
#include <OMUSIC.H>
#include <OUNIT.H>
#include <OTORNADO.H>
#include <OWARPT.H>
#include <OFIRMDIE.H>
#include <OTWALRES.H>
#include <OCAMPGN.H>
#include <OITEMRES.H>
#include <OTIPS.H>
#include <OHELP.H>

//---------------- DETECT_SPREAD ----------------//
//
// Spread out 2 tiles on all direction when detecting sprite.
// A bigger value is required for bigger sprite.
// this should be a number big enough to cover the biggest
// sprite in the game.
//
// It would be equal to the size of the biggest
// sprite in the game + 1 (+1 for the difference
// between (next_x & cur_x).
//
//-----------------------------------------------//


char *Game::auto_load_file_name;
#ifdef DEBUG
LongLog* Game::debug_log = NULL;
#endif


//-------- Begin of function Game::Game --------//
//
Game::Game()
{
   init_flag = 0;
   game_mode = GAME_PREGAME;

	// init_remap_table();        // initialize color remap table
	// moved to Sys::init_objects since vga haven't been initializeded there

	auto_load_file_name = NULL;
}
//--------- End of function Game::Game ---------//


//-------- Begin of function Game::~Game --------//
//
Game::~Game()
{
	err_when( cur_campaign );		// it should have been deinitialized

	if( auto_load_file_name )
	{
		mem_del( auto_load_file_name );
	}
}
//--------- End of function Game::~Game ---------//


//-------- Begin of function Game::init --------//
//
// Note: all functions called in this function cannot call
//			m.random(). Otherwise, it will cause random seed
//		   sync error.
//
// [int] loadGameCall - weather this function is called
//                      when a game is being loaded
//                      (default: 0)
//
int Game::init(int loadGameCall)
{
	if( init_flag )
		deinit();

	int originalRandomSeed = m.get_random_seed();

	music.stop();

	// ----- set waiting cursor -------- //
	int oldCursor = mouse_cursor.get_icon();

	// ------ change mode ----------//

	sys.change_display_mode(config.display_mode_id);
	mouse_cursor.set_icon(CURSOR_WAITING);

	//------- init game data class ---------//

	char tpictFile[] = DIR_RES"I_TPICT?.RES";
	*(strstr( tpictFile, "?")) = '0' + config.terrain_set;
	image_tpict.init(tpictFile,1,0);		// config.terrain_set dependent, must load before town_res.init and terrain_res.init
	terrain_res.init();
	plant_res.init();

	sprite_res.init();                              // sprite resource object must been initialized after game_set as it relies on game_set for info.
	sprite_frame_res.init();

	raw_res.init();
	firm_res.init();
	firm_die_res.init();
	town_res.init();
	town_wall_res.init();
	hill_res.init();
	snow_res.init();
	rock_res.init();
	explored_mask.init();
   se_res.init1();
	se_res.init2(&se_ctrl);
	talk_res.init();
	item_res.init();
	tips_res.init();

	//------- init game data class ---------//

	nation_array.init();
	firm_array.init();
	firm_die_array.init();
	town_array.init();
	unit_array.init();
	bullet_array.init();
	rebel_array.init();
	spy_array.init();
	site_array.init();
	rock_array.init();
	dirt_array.init();
	effect_array.init();
	tornado_array.init();
	war_point_array.init();
	base_obj_array.init();
	way_point_array.init();

	Place::is_being_deleted = 0;

	//------ init game surface class ----------//

	power.init();
	world.init();
	weather.init();
	random_event.init(originalRandomSeed);
	news_array.init();

	if( !loadGameCall )
	{
		if( game.is_campaign_mode() )
			info.init( game.campaign()->game_year, game.campaign()->game_month, game.campaign()->game_day );
		else
			info.init(INFO_DEFAULT_YEAR, INFO_DEFAULT_MONTH, INFO_DEFAULT_DAY);   // it reads in the panel texture and copy it to vga_back's buffer
	}

	if( !loadGameCall )
	{
		//---- init basic end conditions -----//

		win_require_all_condition=0;
		win_condition_count=0;
		lose_condition_count=0;
		win_deadline=0;

		add_win_condition( E_DEFEAT_ALL_OTHER_KINGDOMS, 1 );
		add_lose_condition( E_DEFEAT_OWN_KINGDOM, 1 );
	}

	//---------------------------------------------//

	err_when( originalRandomSeed != m.get_random_seed() );

	mouse_cursor.restore_icon(oldCursor);

	game_has_ended = 0;

	result_win_condition_id = -1;
	result_lose_condition_id = -1;
	result_defeated = 0;
	total_gem_stones = 0;
	max_ranking = 0;
	memset( player_ranking, 0, sizeof(player_ranking) );
	generated_serial = 0;

	init_flag=1;

	return TRUE;
}
//--------- End of function Game::init ---------//


//-------- Begin of function Game::deinit --------//
//
// [int] loadGameCall - weather this function is called
//                      when a game is being loaded
//                      (default: 0)
//
void Game::deinit(int loadGameCall)
{
	if( !init_flag )
		return;

	power.disable();     // disable power, which handle mouse inputs

	music.stop();
	audio.stop_wav();

	//----- set waiting cursor -------- //

	int oldCursor = mouse_cursor.get_icon();
	// ####### begin Gilbert 19/2 ########//
	if( vga_front.vptr_dd_buf )		// if quit by alt-f4, surface destroyed
		mouse_cursor.set_icon(CURSOR_WAITING);
	// ####### end Gilbert 19/2 ########//

	//------- deinit game data class ---------//

	nation_array.deinit();
	firm_array.deinit();
	firm_die_array.deinit();
   town_array.deinit();
   unit_array.deinit();
   bullet_array.deinit();
	rebel_array.deinit();
	spy_array.deinit();
	region_array.deinit();
	site_array.deinit();
	rock_array.deinit();
	dirt_array.deinit();
	effect_array.deinit();
	tornado_array.deinit();
	war_point_array.deinit();
	base_obj_array.deinit();
	way_point_array.deinit();

	//------ deinit game surface class -------//

	world.deinit();
	news_array.deinit();

	if( !loadGameCall )
		info.deinit();

	//------- deinit game data class ---------//

	image_tpict.deinit();
	terrain_res.deinit();
	plant_res.deinit();

	sprite_res.deinit();
	sprite_frame_res.deinit();

	raw_res.deinit();
	firm_res.deinit();
	firm_die_res.deinit();
	town_res.deinit();
	town_wall_res.deinit();
	hill_res.deinit();
	snow_res.deinit();
	rock_res.deinit();
	explored_mask.deinit();
	se_res.deinit();
	talk_res.deinit();
	item_res.deinit();
	tips_res.deinit();

	// ####### begin Gilbert 19/2 ########//
	if( vga_front.vptr_dd_buf )		// if quit by alt-f4, surface destroyed
	{
		//----- restore from waiting cursor -------- //

		mouse_cursor.restore_icon(oldCursor);

		// ------ change mode ----------//
		mouse.hide();
		sys.change_display_mode(MODE_ID_DEFAULT);
		mouse.show();
	}
	// ####### end Gilbert 19/2 ########//

	init_flag=0;
}
//--------- End of function Game::deinit ---------//


//--------- Begin of function Game::init_remap_table --------//

void Game::init_remap_table()
{
	//------------- Define constant ------------//

	#define FIRST_REMAP_KEY       0xE0        // the source color code of the colors to be remapped
															// if change, remember also in oinfo.cpp 
	#define REMAP_KEY_COUNT       8

	//-------- define color remap scheme -------//

	static ColorRemapMethod remap_method_array[] =
	{
      { 0xBC, 0xDC },   // the first remap table is for independent units
      { 0xA0, 0xC0 },   // following are eight remap table for each color code
      { 0xA4, 0xC4 },
      { 0xA8, 0xC8 },
      { 0xAC, 0xCC },
      { 0xB0, 0xD0 },
		{ 0xB4, 0xD4 },
      { 0xB8, 0xD8 },
      { 0xBC, 0xDC },
   };

   //---- define the main color code for each color scheme ----//

   static int main_color_array[] =
	{
      0xDC,
      0xC0,
      0xC4,
      0xC8,
      0xAD,//0xCC,
      0xD0,
      0xD4,
      0xD8,
	};
	//---- define the customized color code for overriding the STD.COL ----//
	//---- such table also exists in oinfo.cpp ----------------------------//
	static BYTE customized_color_code[] =
   {
		186,186,186,	//Gray
		164,164,164,
		141,141,141,
		119,119,119,
		 97, 97, 97,
		 75, 75, 75,
		 52, 52, 52,
		 30, 30, 30,
	
		255,119, 92,	//Red
		244, 85, 77,
		225, 62, 62,
		210, 43, 43,
		181, 34, 34,
		150, 17, 17,
		115, 13, 10,
		 77,  6,  4,

		127,145,254,	//Blue
		100,119,231,
		 79, 95,212,
		 61, 76,188,
		 48, 58,159,
		 33, 41,130,
		 17, 23,103,
		  2,  5, 77,
		 		 
	   107,198, 67,	//Green
		 86,184, 47,
		 74,160, 47,
		 61,140, 45,
		 49,121, 44,
		 37,100, 43,
		 25, 78, 43,
		 13, 54, 39,
		
		239,132,236,	//Purple	
		222,111,217,
		206, 89,201,
		194, 57,188,
		155, 53,150,
		123, 41,121,
 		 94, 27, 93,
		 66, 13, 65,
								
		255,177,  0,	//Orange	
		251,158,  6,
		232,135,  5,
		211,117,  4,
		195,102,  8,
		166, 83,  2,
		151, 66,  8,
		130, 52, 13,
						
		241,232, 44,	//Yellow	
		223,210, 40,
		196,185, 37,
		176,160, 39,
		163,151, 36,
		147,131, 36,
		121,108, 32,
		 96, 83, 27,
										
		214,153,107,	//Brown
		198,128, 85,
		177,103, 66,
		154, 81, 55,
		139, 80, 66,
		112, 62, 55,
		 82, 46, 45,
		 50, 25, 25,
		
		186,186,186,	//Gray
		164,164,164,
		141,141,141,
		119,119,119,
		 97, 97, 97,
		 75, 75, 75,
		 52, 52, 52,
		 30, 30, 30,
	};

	//-------- initialize color remap table -------//

	int         i, j;
	ColorRemap *colorRemap = color_remap_array;

	for( i=0 ; i<=MAX_COLOR_SCHEME ; i++, colorRemap++ )    // MAX_COLOR_SCHEME+1, +1 for independent units
	{
      colorRemap->main_color = main_color_array[i];
		colorRemap->main_color_w = vga.translate_color(colorRemap->main_color);

      for( j=0 ; j<256 ; j++ )
			colorRemap->color_table[j] = vga.translate_color(j);

		for( j=0 ; j<8 ; j++ )
			colorRemap->color_table[FIRST_REMAP_KEY+j] = (short) vga.make_pixel(customized_color_code[i*24+j*3], customized_color_code[i*24+j*3+1], customized_color_code[i*24+j*3+2]);

//      for( j=0 ; j<4 ; j++ )
//			colorRemap->color_table[FIRST_REMAP_KEY+j] = vga.translate_color( (remap_method_array[i].primary_color+j) );

//      for( j=0 ; j<4 ; j++ )
//			colorRemap->color_table[FIRST_REMAP_KEY+4+j] = vga.translate_color( (remap_method_array[i].secondary_color+j) );
   }
}
//---------- End of function Game::init_remap_table --------//

//--------- Begin of function Game::get_color_remap_table --------//
//
// <int> nationRecno  - 0 for independent nation, negative for colorSchemeId
// <int> selectedFlag - whether display outline around the bitmap
//
short* Game::get_color_remap_table(int nationRecno, int selectedFlag)
{
   ColorRemap* colorRemap;
   short* colorRemapTable;

	// ####### begin gilbert 12/3 #######//
	if( nationRecno < 0 )		// negative color scheme id
		colorRemap = color_remap_array - nationRecno;
   else if( nationRecno==0 || nation_array.is_deleted(nationRecno) )    // independent units
	// ####### end gilbert 12/3 #######//
      colorRemap = color_remap_array;
   else
      colorRemap = color_remap_array+nation_array[nationRecno]->color_scheme_id;

   colorRemapTable = colorRemap->color_table;

   //--------- set outline color ---------//

	#define FIRST_CYCLE_COLOR     0xEF
	#define CYCLE_COLOR_COUNT     1
   #define CYCLE_FRAME_INTERVAL  3

   if( selectedFlag )
   {
      int cycleId=0;
/*
      if( CYCLE_COLOR_COUNT==1 )
      {
         cycleId = 0;
      }
      else
      {
         cycleId = sys.frame_count / CYCLE_FRAME_INTERVAL % (CYCLE_COLOR_COUNT*2-2);

         if( cycleId >= CYCLE_COLOR_COUNT )              // cycle in reserved order
            cycleId = CYCLE_COLOR_COUNT*2-2 - cycleId;
      }
*/
      // colorRemapTable[OUTLINE_CODE]        = FIRST_CYCLE_COLOR + cycleId;
      // colorRemapTable[OUTLINE_SHADOW_CODE] = FIRST_CYCLE_COLOR + cycleId;
   }
   else
   {
      // colorRemapTable[OUTLINE_CODE] = (char) TRANSPARENT_CODE;
      // colorRemapTable[OUTLINE_SHADOW_CODE] = (char) SHADOW_CODE;
   }

   return colorRemapTable;
}
//--------- End of function Game::get_color_remap_table --------//
