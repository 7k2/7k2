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

//Filename   : OC_EAS12.CPP
//Description: CampaignEastWest - STAGE_FRYHTAN_RAGE_ATTACK

#include <ALL.H>
#include <OSITE.H>
#include <OTECH.H>
#include <OGAME.H>
#include <OTECHRES.H>
#include <OSTATE.H>
#include <OGODRES.H>
#include <ORACERES.H>
#include <ONEWS.H>
#include <OMONSRES.H>
#include <ONATION2.H>
#include <OC_EAST.H>
#include <OF_CAMP.H>

//----- define constant ----------//

enum { STAGE_12_MONSTER_NATION_RECNO = 2  };


//---- Begin of function CampaignEastWest::stage_12_create_game ----//
//
// Vars defined in event functions before calling this function:
//
// -cur_monster_nation_recno    - the attacking monster nation recno
// -attacker_state_recno	     - the monster attacking state
// -target_state_recno			  - the player target state
//
int CampaignEastWest::stage_12_create_game()
{
	init_random_plot('D');

	//--- randomly init the current situation -----//

	set_cur_situation( m.random(CAMPAIGN_SITUATION_COUNT)+1 );

	//------- create objects and vars --------//

	if( !stage_12_create_nation() )
		return 0;

	if( !stage_12_create_town() )
		return 0;

	if( !stage_12_create_firm() )
		return 0;

	//------ create monsters ---------//

	create_troop(STAGE_12_MONSTER_NATION_RECNO, campaign_difficulty*15);

	//------ init stage 1 vars -------//

	stage_12_init_vars();

	//---- generate plants and other objects on the map -----//

	world.gen_rocks();		// generate mountains

	world.generate_map2();

	//------- create additional raw sites ----------//

	create_raw_site();

	//------- init relationship -------//

	stage_12_init_relation();

	// ####### begin Gilbert 9/4 ########//
	// ----- create royal units ------//

	create_royal_units(CAMPAIGN_PLAYER_NATION_RECNO);
	// ####### end Gilbert 9/4 ########//

	//------ plot create game --------//

	(this->*plot_create_game_FP)();

	return 1;
}
//---- End of function CampaignEastWest::stage_12_create_game -----//


//---- Begin of function CampaignEastWest::stage_12_init_vars ----//

void CampaignEastWest::stage_12_init_vars()
{
	//----- init monster reinforcement settings -----//

	monster_reinforcement_count = campaign_difficulty*10;

	next_monster_reinforcement_date = info.game_date + (MAX_CAMPAIGN_DIFFICULTY-campaign_difficulty+1) * 30;

	//----- set player nation vars -----//

	init_player_resource(65);

	init_nation_resource(STAGE_12_MONSTER_NATION_RECNO, 80);
}
//---- End of function CampaignEastWest::stage_12_init_vars -----//


//---- Begin of function CampaignEastWest::stage_12_prelude ----//
//
// Pick a state to attack.
//
void CampaignEastWest::stage_12_prelude()
{
	int raceId = get_nation( state_array[attacker_state_recno]->campaign_nation_recno )->race_id;

	disp_dialog( raceId, res_stage.read("12PRELUD"), INFO_REPAINT );

	attack_state( attacker_state_recno, target_state_recno, 0, 1, 2 );

	//------- set config settings -------//

	config.explore_whole_map = 1;
}
//---- End of function CampaignEastWest::stage_12_prelude -----//


//---- Begin of function CampaignEastWest::stage_12_create_nation ----//

int CampaignEastWest::stage_12_create_nation()
{
	//-------- create the player nation ------//

	state_array[target_state_recno]->add_game_nation();

	//------- create monster nation --------//

	state_array[attacker_state_recno]->add_game_nation();

	return 1;
}
//---- End of function CampaignEastWest::stage_12_create_nation ----//


//---- Begin of function CampaignEastWest::stage_12_create_town ----//

int CampaignEastWest::stage_12_create_town()
{
	//------- set the range of placements of objects -------//

	enum { TOWN_STATE_MAP_WIDTH = 100,
			 TOWN_STATE_MAP_HEIGHT = 100 };

	//------ generate a StateArray ---------//

	int townAddCount  = 8 + m.random(3);		// add 8 to 10 player towns
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

		if( addedCount%2==0 )
			soldierAddCount = MAX_SOLDIER/4 + m.random(MAX_SOLDIER/4) + 1;
		else
			soldierAddCount = MAX_SOLDIER/2 + m.random(MAX_SOLDIER/2) + 1;

		if( create_nation_town(PLAYER_NATION_RECNO, xLoc, yLoc, 1, 0, soldierAddCount) )		// 0-the race of the town need not to be the same as the nation
			addedCount++;
	}

	town_array.update_all_camp_link();

	return 1;
}
//---- End of function CampaignEastWest::stage_12_create_town ----//


//---- Begin of function CampaignEastWest::stage_12_create_firm ----//

int CampaignEastWest::stage_12_create_firm()
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

	if( !create_economic_firm(townRecno1) )
		return 0;

	if( townRecno2 && !create_military_firm(townRecno2) )
		return 0;

	//------ create additional military/economic firms ------//

	int addCount = 1+m.random(2);		// 1 to 2

	for( int i=0 ; i<addCount ; i++ )
	{
		int townRecno = random_pick_town_with_camp(PLAYER_NATION_RECNO, 2);		// only pick towns with <= 2 links
		int rc;

		if( !townRecno )
      	break;

		if( has_situation )
		{
			rc = m.random( cur_situation.economy_level+cur_situation.military_level )
				  < cur_situation.economy_level;
		}
		else
		{
			rc = m.random(2)==0;
		}

		if( rc )
			rc = create_economic_firm(townRecno);
		else
			rc = create_military_firm(townRecno, 30+m.random(30));		// power rating: 30 to 60

		if( !rc )
			break;
	}

	return 1;
}
//---- End of function CampaignEastWest::stage_12_create_firm ----//


//------- Begin of function CampaignEastWest::stage_12_init_relation -------//
//
void CampaignEastWest::stage_12_init_relation()
{
	set_nation_hostile(PLAYER_NATION_RECNO, STAGE_12_MONSTER_NATION_RECNO);
}
//------- End of function CampaignEastWest::stage_12_init_relation --------//


//=================================================================//


//------- Begin of function CampaignEastWest::stage_12_next_day -------//
//
void CampaignEastWest::stage_12_next_day()
{
	//------ plot next day ----------//

	(this->*plot_next_day_FP)();

	//-------------------------------//

	if( !nation_array.is_deleted(STAGE_12_MONSTER_NATION_RECNO) )
	{
		think_enemy_attack(STAGE_12_MONSTER_NATION_RECNO, PLAYER_NATION_RECNO);

		monster_reinforcement_to_lair();

		monster_reinforcement(STAGE_12_MONSTER_NATION_RECNO);
	}
}
//------- End of function CampaignEastWest::stage_12_next_day --------//


//============================================================//


//--- Begin of function CampaignEastWest::stage_12_process_game_result ---//
//
void CampaignEastWest::stage_12_process_game_result()
{
	//--- if the player won the game -----//

	if( game.result_win_condition_id >= 0 )
	{
		set_event( EVENT_RANDOM_POINT_2 );
	}

	//---- if the player lost the game ----//

	else
	{
		attack_state( attacker_state_recno, target_state_recno, 1, 2, 3 );

		set_event( EVENT_RANDOM_POINT_2 );
	}
}
//---- End of function CampaignEastWest::stage_12_process_game_result -----//


//------ Begin of function CampaignEastWest::stage_12_disp_end_game_msg ------//
//
void CampaignEastWest::stage_12_disp_end_game_msg()
{
	if( game.result_win_condition_id >= 0 )
	{
		Nation* playerNation = (~nation_array);

		if( game.result_win_condition_id == E_DEFEAT_ALL_OTHER_KINGDOMS )
		{
			disp_in_game_msg( res_stage.read("12WIN") );
		}
		else
			err_here();
	}
	else
	{
		disp_in_game_msg( res_stage.read("12LOSE") );
	}
}
//------- End of function CampaignEastWest::stage_12_disp_end_game_msg -------//

