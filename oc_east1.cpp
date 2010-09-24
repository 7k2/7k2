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
//Description: CampaignEastWest - STAGE_FRYHTAN_INVASION_1

#include <ALL.H>
#include <OSITE.H>
#include <OTECH.H>
#include <OGAME.H>
#include <OTECHRES.H>
#include <OSTATE.H>
#include <OUNIT.H>
#include <OGODRES.H>
#include <OUN_GRP.H>
#include <ORACERES.H>
#include <ONEWS.H>
#include <OMONSRES.H>
#include <ONATION2.H>
#include <OC_EAST.H>
#include <OF_CAMP.H>

//----- define constant ----------//

enum { STAGE_1_MONSTER_NATION_RECNO = 2  };


//---- Begin of function CampaignEastWest::stage_1_create_game ----//
//
// Vars defined in event functions before calling this function:
//
// None
//
int CampaignEastWest::stage_1_create_game()
{
	// ###### begin Gilbert 9/4 ########//
	init_random_plot(0);		// no plot
	// ###### end Gilbert 9/4 ########//

	if( !stage_1_create_nation() )
		return 0;

	if( !stage_1_create_town() )
		return 0;

	if( !stage_1_create_firm() )
		return 0;

	//------ create monsters ---------//

	int unitCountArray[MAX_CAMPAIGN_DIFFICULTY] = { 6, 10, 22, 31, 42 };

	create_troop(STAGE_1_MONSTER_NATION_RECNO, unitCountArray[campaign_difficulty-1] );

	if( unit_group.size() > 0 )		// when a troop is created successfully in create_troop(), unit_group will keep the created units
	{
		//--- create a unit for building

		Unit* unitPtr = unit_group.get_unit(1);
		int   xLoc = unitPtr->cur_x_loc();
		int   yLoc = unitPtr->cur_y_loc();

		int unitRecno = create_unit( STAGE_1_MONSTER_NATION_RECNO,
							 unitPtr->unit_id, RANK_SOLDIER, xLoc, yLoc, xLoc, yLoc );
	}

	//------ init stage 1 vars -------//

	stage_1_init_vars();

	//---- generate plants and other objects on the map -----//

	world.gen_rocks();		// generate mountains

	world.generate_map2();

	//------- create additional raw sites ----------//

	create_raw_site();

	//------- init relationship -------//

	stage_1_init_relation();

	// ####### begin Gilbert 9/4 ########//
	// ----- create royal units ------//

	create_royal_units(CAMPAIGN_PLAYER_NATION_RECNO);
	// ####### end Gilbert 9/4 ########//

	//------ plot create game --------//

	// ###### begin Gilbert 28/3 ########//
	(this->*plot_create_game_FP)();
	// ###### end Gilbert 28/3 ########//

	return 1;
}
//---- End of function CampaignEastWest::stage_1_create_game -----//


//---- Begin of function CampaignEastWest::stage_1_init_vars ----//

void CampaignEastWest::stage_1_init_vars()
{
	init_player_resource(30);

	init_nation_resource(STAGE_1_MONSTER_NATION_RECNO, 20);
}
//---- End of function CampaignEastWest::stage_1_init_vars -----//


//---- Begin of function CampaignEastWest::stage_1_prelude ----//
//
// Pick a state to attack.
//
void CampaignEastWest::stage_1_prelude()
{
	cur_monster_nation_recno = random_pick_monster_campaign_nation();

	int rc = random_pick_attack_state(cur_monster_nation_recno, western_nation_recno);

	err_when( !rc );

	play_speech("PRE-01.WAV");
	disp_narrative( res_stage.read("1PRELUDE") );		// 1-the letter is from a Fryhtan kingdom
	stop_speech();

	attack_state( attacker_state_recno, target_state_recno, 0, 1, 2 );

	//------- set config settings -------//

	config.explore_whole_map = 1;
}
//---- End of function CampaignEastWest::stage_1_prelude -----//


//---- Begin of function CampaignEastWest::stage_1_create_nation ----//

int CampaignEastWest::stage_1_create_nation()
{
	//-------- create the player nation ------//

	state_array[target_state_recno]->add_game_nation();

	//------- create monster nation --------//

	state_array[attacker_state_recno]->add_game_nation();

	return 1;
}
//---- End of function CampaignEastWest::stage_1_create_nation ----//


//---- Begin of function CampaignEastWest::stage_1_create_town ----//

int CampaignEastWest::stage_1_create_town()
{
	//------- set the range of placements of objects -------//

	enum { TOWN_STATE_MAP_WIDTH = 100,
			 TOWN_STATE_MAP_HEIGHT = 100 };

	//------ generate a StateArray ---------//

	int townAddCount  = 4 + m.random(2);		// add 4 to 5 player towns
	int addedCount=0;

	StateArray townStateArray;

	townStateArray.init(TOWN_STATE_MAP_WIDTH, TOWN_STATE_MAP_HEIGHT);
	townStateArray.generate(13);

	for( int i=townStateArray.size() ; i>0 && addedCount<townAddCount ; i-- )
	{
		StateInfo* stateInfo = townStateArray[i];

		int xLoc = (int) stateInfo->center_x * MAX_WORLD_X_LOC / TOWN_STATE_MAP_WIDTH;
		int yLoc = (int) stateInfo->center_y * MAX_WORLD_Y_LOC / TOWN_STATE_MAP_HEIGHT;

		int soldierAddCount;

		if( addedCount==0 )		// the first town
			soldierAddCount = MAX_SOLDIER;
		else
			soldierAddCount = MAX_SOLDIER/2 + m.random(MAX_SOLDIER/2) + 1;

		if( create_nation_town(PLAYER_NATION_RECNO, xLoc, yLoc, 1, 1, soldierAddCount) )		// 1-the race of the town should be the same as the nation
			addedCount++;
	}

	town_array.update_all_camp_link();

	return 1;
}
//---- End of function CampaignEastWest::stage_1_create_town ----//


//---- Begin of function CampaignEastWest::stage_1_create_firm ----//

int CampaignEastWest::stage_1_create_firm()
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
//---- End of function CampaignEastWest::stage_1_create_firm ----//


//------- Begin of function CampaignEastWest::stage_1_init_relation -------//
//
void CampaignEastWest::stage_1_init_relation()
{
	set_nation_hostile(PLAYER_NATION_RECNO, STAGE_1_MONSTER_NATION_RECNO);
}
//------- End of function CampaignEastWest::stage_1_init_relation --------//


//=================================================================//


//------- Begin of function CampaignEastWest::stage_1_next_day -------//
//
void CampaignEastWest::stage_1_next_day()
{
	// ###### begin Gilbert 28/3 ########//
	(this->*plot_next_day_FP)();
	// ###### end Gilbert 28/3 ########//
}
//------- End of function CampaignEastWest::stage_1_next_day --------//


//============================================================//


//--- Begin of function CampaignEastWest::stage_1_process_game_result ---//
//
void CampaignEastWest::stage_1_process_game_result()
{
	//--- if the player won the game -----//

	if( game.result_win_condition_id >= 0 )
	{
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
//---- End of function CampaignEastWest::stage_1_process_game_result -----//


//------ Begin of function CampaignEastWest::stage_1_disp_end_game_msg ------//
//
void CampaignEastWest::stage_1_disp_end_game_msg()
{
	if( game.result_win_condition_id >= 0 )
	{
		disp_in_game_msg( res_stage.read("1WIN"), (int) (~nation_array)->kill_monster_score );
	}
	else
	{
		disp_in_game_msg( res_stage.read("1LOSE") );
	}
}
//------- End of function CampaignEastWest::stage_1_disp_end_game_msg -------//

