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
//Description: CampaignEastWest - STAGE_EASTERN_EMPIRE_INVADE_OUR_LAND

#include <all.h>
#include <osite.h>
#include <otech.h>
#include <ogame.h>
#include <otechres.h>
#include <ostate.h>
#include <ogodres.h>
#include <oun_grp.h>
#include <oraceres.h>
#include <onews.h>
#include <omonsres.h>
#include <onation2.h>
#include <oc_east.h>
#include <of_camp.h>

//----- define constant ----------//

enum { STAGE_9_EASTERN_NATION_RECNO = 2  };

//---- Begin of function CampaignEastWest::stage_9_create_game ----//
//
// Vars defined in event functions before calling this function:
//
// -cur_monster_nation_recno
// -attack_state_recno
// -target_state_recno
//
int CampaignEastWest::stage_9_create_game()
{
	init_random_plot('D');

	if( !stage_9_create_nation() )
		return 0;

	if( !stage_9_create_town() )
		return 0;

	if( !stage_9_create_firm() )
		return 0;

	//---- create the invading force from the eastern empire ----//

	create_troop(STAGE_9_EASTERN_NATION_RECNO, 10+campaign_difficulty*12);

	create_wagon(STAGE_9_EASTERN_NATION_RECNO, 10+campaign_difficulty*15+misc.random(10), 1);		// 1-useLastLocation

	//------ init stage 9 vars -------//

	stage_9_init_vars();

	//---- generate plants and other objects on the map -----//

	world.gen_rocks();		// generate mountains

	world.generate_map2();

	//------- create additional raw sites ----------//

	create_raw_site();

	//------- init relationship -------//

	stage_9_init_relation();

	// ####### begin Gilbert 9/4 ########//
	// ----- create royal units ------//

	create_royal_units(CAMPAIGN_PLAYER_NATION_RECNO);

	//------ plot create game --------//

	(this->*plot_create_game_FP)();
	// ####### end Gilbert 9/4 ########//

	return 1;
}
//---- End of function CampaignEastWest::stage_9_create_game -----//


//---- Begin of function CampaignEastWest::stage_9_init_vars ----//

void CampaignEastWest::stage_9_init_vars()
{
	//------ init stage 9 vars -------//

	rebel_count = misc.random(campaign_difficulty/2);
	next_rebel_date = info.game_date + misc.random(300);

	general_defect_count = misc.random(campaign_difficulty/2);
	next_general_defect_date = info.game_date + misc.random(300);

	init_player_resource(50);

	init_nation_resource(STAGE_9_EASTERN_NATION_RECNO, 60);
}
//---- End of function CampaignEastWest::stage_9_init_vars -----//


//---- Begin of function CampaignEastWest::stage_9_prelude ----//
//
// Pick a state to attack.
//
void CampaignEastWest::stage_9_prelude()
{
	play_speech("PRE-09.WAV");
	disp_narrative( res_stage.read("9PRELUDE") );
	stop_speech();

	attack_state( attacker_state_recno, target_state_recno, 0, 1, 2 );

	//------- set config settings -------//

	config.explore_whole_map = 1;
}
//---- End of function CampaignEastWest::stage_9_prelude -----//


//---- Begin of function CampaignEastWest::stage_9_create_nation ----//

int CampaignEastWest::stage_9_create_nation()
{
	//-------- create the player nation ------//

	state_array[target_state_recno]->add_game_nation();

	//------- create monster nation --------//

	state_array[attacker_state_recno]->add_game_nation();

	return 1;
}
//---- End of function CampaignEastWest::stage_9_create_nation ----//


//---- Begin of function CampaignEastWest::stage_9_create_town ----//

int CampaignEastWest::stage_9_create_town()
{
	//------- set the range of placements of objects -------//

	enum { TOWN_STATE_MAP_WIDTH = 100,
			 TOWN_STATE_MAP_HEIGHT = 100 };

	//------ generate a StateArray ---------//

	int townAddCount  = 4 + misc.random(2);		// add 4 to 5 player towns
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

		if( addedCount==0 )
			soldierAddCount = MAX_SOLDIER;
		else
			soldierAddCount = MAX_SOLDIER/2 + misc.random(MAX_SOLDIER/2) + 1;

		if( create_nation_town(PLAYER_NATION_RECNO, xLoc, yLoc, 1, 1, soldierAddCount) )		// 1-the race of the town should be the same as the nation
			addedCount++;
	}

	//----- create independent towns ------//

	Battle::create_independent_town(4+misc.random(3));

   //-------- update links -------//

	town_array.update_all_camp_link();

	return 1;
}
//---- End of function CampaignEastWest::stage_9_create_town ----//


//---- Begin of function CampaignEastWest::stage_9_create_firm ----//

int CampaignEastWest::stage_9_create_firm()
{
	//----- create firms around established towns -----//

	Nation* nationPtr = nation_array[PLAYER_NATION_RECNO];

	int townRecno1 = king_oversee_town_recno(PLAYER_NATION_RECNO);
	int townRecno2 = random_pick_town_with_camp(PLAYER_NATION_RECNO, 2);		// only pick towns with <= 2 links

	if( townRecno2 && misc.random(2)==0 )		// swap the town in 50% chance
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
//---- End of function CampaignEastWest::stage_9_create_firm ----//


//------- Begin of function CampaignEastWest::stage_9_init_relation -------//
//
void CampaignEastWest::stage_9_init_relation()
{
	set_nation_hostile(PLAYER_NATION_RECNO, STAGE_9_EASTERN_NATION_RECNO);
}
//------- End of function CampaignEastWest::stage_9_init_relation --------//


//=================================================================//


//------- Begin of function CampaignEastWest::stage_9_next_day -------//
//
void CampaignEastWest::stage_9_next_day()
{
	// ###### begin Gilbert 9/4 ########//
	(this->*plot_next_day_FP)();
	// ###### end Gilbert 9/4 ########//

	if( !nation_array.is_deleted(STAGE_9_EASTERN_NATION_RECNO) )
	{
		think_enemy_attack(STAGE_9_EASTERN_NATION_RECNO, PLAYER_NATION_RECNO);

		if( should_rebel(150) )
			town_defect(PLAYER_NATION_RECNO);

		if( should_general_defect(300) )
			general_defect_form_new_nation(PLAYER_NATION_RECNO);
	}
}
//------- End of function CampaignEastWest::stage_9_next_day --------//


//============================================================//


//--- Begin of function CampaignEastWest::stage_9_process_game_result ---//
//
void CampaignEastWest::stage_9_process_game_result()
{
	//--- if the player won the game -----//

	if( game.result_win_condition_id >= 0 )
	{
		defeated_eastern_empire_invasion = 1;

		set_event(EVENT_RANDOM_POINT_2);
	}

	//---- if the player lost the game ----//

	else
	{
		attack_state( attacker_state_recno, target_state_recno, 1, 2, 3 );

		set_event(EVENT_RANDOM_POINT_2);
	}
}
//---- End of function CampaignEastWest::stage_9_process_game_result -----//


//------ Begin of function CampaignEastWest::stage_9_disp_end_game_msg ------//
//
void CampaignEastWest::stage_9_disp_end_game_msg()
{
	if( game.result_win_condition_id >= 0 )
	{
		disp_in_game_msg( res_stage.read("9WIN") );
	}
	else
	{
		disp_in_game_msg( res_stage.read("9LOSE") );
	}
}
//------- End of function CampaignEastWest::stage_9_disp_end_game_msg -------//

