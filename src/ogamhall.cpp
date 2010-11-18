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

//Filename    : OGAMHALL.CPP
//Description : Hall of Fame

#include <ovga.h>
#include <ovgalock.h>
#include <odate.h>
#include <ostr.h>
#include <osys.h>
#include <ofont.h>
#include <omouse.h>
#include <oimgres.h>
#include <oconfig.h>
#include <oraceres.h>
#include <ogame.h>
#include <onation.h>
#include <ogfile.h>
#include <ot_unit.h>
#include <ot_gmenu.h>

//------ Begin of function GameFileArray::disp_hall_of_fame -----//
//
// Display the Hall of Fame
// <int> type = 0 when human
//				  = 1 when frythan	
//
void GameFileArray::disp_hall_of_fame(int type)
{
	// ######## begin Gilbert 10/6 #######//
	// read hall of fame first
	if( !has_read_hall_of_fame )
	{
		read_hall_of_fame();
		has_read_hall_of_fame = 1;
	}
	// ######## end Gilbert 10/6 #######//

	int y = 116, offset = 76;
	if (type)
	{
		vga.disp_image_file("HALFAME2");
		y += 76;
		offset -= 20;
	}
	else
		vga.disp_image_file("HALFAME1");

	//---------- display hall of fame records ------------//

	int i;
	int x=130;

	for( i=0 ; i<HALL_FAME_NUM ; i++, y+=offset )
	{
		hall_fame_array[type][i].disp_info( x, y, i+1 );
	}

	mouse.wait_press(60);		// 60 seconds to time out

	vga.finish_disp_image_file();
}
//------- End of function GameFileArray::disp_hall_of_fame -----//


//------ Begin of function HallFame::disp_info -------//
//
// Display a Hall of Fame record
//
// <int> x, y = the location of the information
// <int> pos  = the position of the record.
//
void HallFame::disp_info(int x, int y, int pos)
{
	if( !start_year )    // no information
		return;

	//------------------------------------------------------//
	//
	// e.g. 1. [Image] King Trevor Chan
	//    	  [     ] Score : 150    Population : 1000    Period : 1001-1030
	//
	//------------------------------------------------------//

	Font* fontPtr;

	fontPtr = &font_hall;

	String str;
	int    y2 = y+17;

	//----------------------------------------//

	str  = pos;
	str += ".";

	fontPtr->put( x, y, str );

	x += 16;

	//----------------------------------------//

//	str  = translate.process("King ");
//	str += player_name;
	str  = text_unit.str_unit_titled( race_id, player_name, RANK_KING );

	fontPtr->put( x, y, str );

	//----------------------------------------//

//	str  = translate.process("Score: ");
	str  = text_game_menu.str_hall_of_fame_score();
	str += score;

	fontPtr->put( x, y2, str );

	//----------------------------------------//

	int xPop, xPeriod, xDiff;

	xPop = x + 110;
	#if(defined(FRENCH))
		xPop -= 6;
	#endif
	xPeriod = xPop + 145; // x + 255;
	#if(defined(GERMAN))
		xPeriod -= 10;
	#endif
	xDiff = xPeriod + 155; // x + 410;
	#if(defined(FRENCH))
		xDiff += 6;
	#endif

	// str  = translate.process("Population: ");
	str  = text_game_menu.str_hall_of_fame_population();
	str += population;

	fontPtr->put( xPop, y2, str );

	//----------------------------------------//

	// str  = translate.process("Period: ");
	str  = text_game_menu.str_hall_of_fame_period();
	str += m.num_to_str(start_year);     // without adding comma separators
	str += "-";
	str += m.num_to_str(end_year);

	fontPtr->put( xPeriod, y2, str );

	//----------------------------------------//

	// str  = translate.process("Difficulty: ");
	str  = text_game_menu.str_hall_of_fame_difficulty();
	str += difficulty_rating;

	fontPtr->put( xDiff, y2, str );
}
//------- End of function HallFame::disp_info -------//


//------ Begin of function GameFileArray::add_hall_of_fame -----//
//
// Add current game into the hall of hame
//
// <int> totalScore of the player.
//
// <int> type = 0 when human
//				  = 1 when frythan	
//
// return : <int> 1-hall of fame updated
//                0-not updated
//
int GameFileArray::add_hall_of_fame(int totalScore, int type)
{
   //-------- insert the record -----------//

   int i;

   for( i=0 ; i<HALL_FAME_NUM ; i++ )
   {
      if( totalScore > hall_fame_array[type][i].score )
      {
         //---------- move and insert the data --------//

         if( i < HALL_FAME_NUM-1 )      // it is not the last record
         {
				// ###### begin Gilbert 13/4 ########//
				memmove( &hall_fame_array[type][i+1], &hall_fame_array[type][i],
					sizeof(HallFame) * (HALL_FAME_NUM-i-1) );
				// ###### end Gilbert 13/4 ########//
         }

         //-------- record the hall of fame rcord ------//

         hall_fame_array[type][i].record_data(totalScore);

         //--------- display the hall of fame ----------//

		   write_hall_of_fame();        // must write hall of fame, because it also write the last saved game slot no.

         disp_hall_of_fame(type);
         return 1;
      }
   }

   return 0;
}
//------- End of function GameFileArray::add_hall_of_fame -----//


//--------- Begin of function HallFame::record_data --------//
//
// Record the hall of fame record_data
//
void HallFame::record_data(int totalScore)
{
	Nation* playerNation = ~nation_array;

	strncpy( player_name, playerNation->king_name(), NationArray::CUSTOM_NAME_LEN );
	player_name[NationArray::CUSTOM_NAME_LEN] = NULL;

	race_id	  = playerNation->race_id;

	start_year = date.year(info.game_start_date);
	end_year   = info.game_year;

	score  	  = totalScore;
	population = playerNation->all_population();

	difficulty_rating = race_id >= 0 ? config.human_difficulty_rating : config.monster_difficulty_rating;
}
//----------- End of function HallFame::record_data ---------//
