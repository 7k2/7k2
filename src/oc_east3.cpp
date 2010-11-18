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

//Filename   : OC_EAST.CPP
//Description: CampaignEastWest - STAGE_BUILD_UP

#include <all.h>
#include <osite.h>
#include <otech.h>
#include <ogame.h>
#include <otechres.h>
#include <ostate.h>
#include <ogodres.h>
#include <oraceres.h>
#include <onews.h>
#include <omonsres.h>
#include <onation2.h>
#include <oc_east.h>
#include <ofirm.h>


//---- Begin of function CampaignEastWest::stage_3_create_game ----//
//
// Vars defined in event functions before calling this function:
//
// None
//
int CampaignEastWest::stage_3_create_game()
{
	// ###### begin Gilbert 9/4 ########//
	init_random_plot(0);		// no plot
	// ###### end Gilbert 9/4 ########//

	//--- randomly init the current situation -----//

	set_cur_situation( m.random(CAMPAIGN_SITUATION_COUNT)+1 );

	//------- create objects and vars --------//

	if( !stage_3_create_nation() )
		return 0;

	if( !stage_3_create_town() )
		return 0;

	if( !stage_3_create_firm() )
		return 0;

	//------ init stage 1 vars -------//

	stage_3_init_vars();

	//---- generate plants and other objects on the map -----//

	world.gen_rocks();		// generate mountains

	world.generate_map2();

	//------- create additional raw sites ----------//

	create_raw_site();

	// ####### begin Gilbert 9/4 ########//
	// ----- create royal units ------//

	create_royal_units(CAMPAIGN_PLAYER_NATION_RECNO);

	//------ plot create game --------//

	(this->*plot_create_game_FP)();
	// ####### end Gilbert 9/4 ########//

	return 1;
}
//---- End of function CampaignEastWest::stage_3_create_game -----//


//---- Begin of function CampaignEastWest::stage_3_init_vars ----//

void CampaignEastWest::stage_3_init_vars()
{
	//------ set game event vars -------//

	rebel_count = 1 + campaign_difficulty;		// 1 to 6 rebels will form new nation
	next_rebel_date = info.game_date;			// so that there will be a rebel in the beginning

	//------- set player nation vars -------//

	init_player_resource(50);

	//-------- add victory conditions -------//

	int yearLimit = 30;

	game.add_win_condition( E_REACH_MILITARY, 1, 0, 300, yearLimit );
	game.add_win_condition( E_REACH_ECONOMY , 1, 0, 300, yearLimit );

	game.win_require_all_condition = 1;
}
//---- End of function CampaignEastWest::stage_3_init_vars -----//


//---- Begin of function CampaignEastWest::stage_3_prelude ----//
//
// Pick a state to attack.
//
void CampaignEastWest::stage_3_prelude()
{
	play_speech("PRE-03.WAV");
	disp_narrative( res_stage.read("3PRELUDE") );
	stop_speech();

	target_state_recno = random_pick_state(PLAYER_NATION_RECNO);

	rebel_attack_state( target_state_recno, 0, 1, 2 );

	//------- set config settings -------//

	config.explore_whole_map = 1;
}
//---- End of function CampaignEastWest::stage_3_prelude -----//


//---- Begin of function CampaignEastWest::stage_3_create_nation ----//

int CampaignEastWest::stage_3_create_nation()
{
	//-------- create the player nation ------//

	state_array[target_state_recno]->add_game_nation();

	return 1;
}
//---- End of function CampaignEastWest::stage_3_create_nation ----//


//---- Begin of function CampaignEastWest::stage_3_create_town ----//

int CampaignEastWest::stage_3_create_town()
{
	enum { TOWN_STATE_MAP_WIDTH = 100,
			 TOWN_STATE_MAP_HEIGHT = 100 };

	//------ generate a StateArray ---------//

	int playerTownAddCount  	 = 10 + m.random(3);
	int independentTownAddCount = 3 + m.random(3);

	StateArray townStateArray;

	townStateArray.init(TOWN_STATE_MAP_WIDTH, TOWN_STATE_MAP_HEIGHT);
	townStateArray.generate(30);

	for( int i=townStateArray.size() ; i>0 ; i-- )
	{
		StateInfo* stateInfo = townStateArray[i];

		int xLoc = (int) stateInfo->center_x * MAX_WORLD_X_LOC / TOWN_STATE_MAP_WIDTH;
		int yLoc = (int) stateInfo->center_y * MAX_WORLD_Y_LOC / TOWN_STATE_MAP_HEIGHT;

		//-------- add player towns --------//

		if( playerTownAddCount > 0 )
		{
			int buildFort = (i%3==0);		// 33% have fort and the rest don't. Rebels only happens in towns without forts.

			if( create_nation_town(PLAYER_NATION_RECNO, xLoc, yLoc, buildFort) )
				playerTownAddCount--;
		}

		//----- add independent towns -------//

		else if( independentTownAddCount > 0 )
		{
			int raceId = m.random(MAX_RACE)+1;
			int initPopulation = 30 + m.random(30);
			int initResistance = 60 + m.random(40);

			create_independent_town(xLoc, yLoc, raceId, initPopulation, initResistance);

			independentTownAddCount--; 
		}
	}

	town_array.update_all_camp_link();

	return 1;
}
//---- End of function CampaignEastWest::stage_3_create_town ----//


//---- Begin of function CampaignEastWest::stage_3_create_firm ----//

int CampaignEastWest::stage_3_create_firm()
{
	//----- create firms around established towns -----//

	Nation* nationPtr = nation_array[PLAYER_NATION_RECNO];

	int townRecno1 = king_oversee_town_recno(PLAYER_NATION_RECNO);
	int townRecno2 = random_pick_town_with_camp(PLAYER_NATION_RECNO, 2);		// only pick towns with <= 2 links

	if( townRecno2 && m.random(2)==0 )		// swap the town in 50% chance
	{
		int t=townRecno1;
		townRecno1 = townRecno2;
		townRecno2 = t;
	}

	//--- while the races of towns are random, the two main towns' race has to be the king's race ---//

	town_array[townRecno1]->set_race(nationPtr->race_id);

	if( townRecno2 )
		town_array[townRecno2]->set_race(nationPtr->race_id);

	if( !create_economic_firm(townRecno1) )
		return 0;

	if( townRecno2 && !create_military_firm(townRecno2) )
		return 0;

	return 1;
}
//---- End of function CampaignEastWest::stage_3_create_firm ----//


//=================================================================//


//------- Begin of function CampaignEastWest::stage_3_next_day -------//
//
void CampaignEastWest::stage_3_next_day()
{
	// ###### begin Gilbert 9/4 ########//
	(this->*plot_next_day_FP)();
	// ###### end Gilbert 9/4 ########//

	if( info.game_date < next_rebel_date || rebel_count==0 )
		return;

	//--- if the player is already weak enough, don't further rebel ---//

	if( campaign_difficulty <= 3 &&
		 (~nation_array)->overall_rating < nation_array.max_overall_rating-30 )
	{
		rebel_count=0;
		return;
	}

	//-----------------------------------//

	rebel_count--;

	next_rebel_date += 30 + m.random(30);

	town_defect( PLAYER_NATION_RECNO, 0 );

	if( rebel_count%3==0 ||
		 m.random( (MAX_CAMPAIGN_DIFFICULTY-campaign_difficulty)/2 + nation_array.nation_count - 1 )==0
		 && info.game_date > info.game_start_date + 90 )
	{
		rebel_form_nation();       // when this function is called, a new nation will formed if there is any independent towns.
	}
}
//------- End of function CampaignEastWest::stage_3_next_day --------//


//============================================================//


//--- Begin of function CampaignEastWest::stage_3_process_game_result ---//
//
void CampaignEastWest::stage_3_process_game_result()
{
	//--- if the player won the game -----//

	if( game.result_win_condition_id >= 0 )
	{
		hero_debut_state_count++;
		set_event( EVENT_RANDOM_POINT_1 );
	}

	//---- if the player lost the game ----//

	else
	{
		// do nothing as the player must retry the game.

		info.game_year = game_year;			// revert info.game_year back to campaign's game year so that the game's time will not advance

		m.set_random_seed( saved_random_seed );		// restore the random seed so that the game will replay in the same way
	}
}
//---- End of function CampaignEastWest::stage_3_process_game_result -----//


//------ Begin of function CampaignEastWest::stage_3_disp_end_game_msg ------//
//
void CampaignEastWest::stage_3_disp_end_game_msg()
{
	if( game.result_win_condition_id >= 0 )
	{
		disp_in_game_msg( res_stage.read("3WIN") );
	}
	else
	{
		if( game.result_lose_condition_id == E_DEFEAT_OWN_KINGDOM )
			disp_in_game_msg( res_stage.read("3LOSE1") );
		else
			disp_in_game_msg( res_stage.read("3LOSE2") );
	}
}
//------- End of function CampaignEastWest::stage_3_disp_end_game_msg -------//

