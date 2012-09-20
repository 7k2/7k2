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

//Filename   : OC_EAST6.CPP
//Description: CampaignEastWest - STAGE_HERO_DEBUT

#include <all.h>
#include <itemid.h>
#include <osite.h>
#include <otech.h>
#include <ogame.h>
#include <otechres.h>
#include <ostate.h> 
#include <ogodres.h>
#include <oraceres.h>
#include <oherores.h>
#include <onews.h>
#include <omonsres.h>
#include <onation2.h>
#include <oc_east.h>
#include <of_camp.h>

//----- define constant ----------//

enum { STAGE_6_MONSTER_NATION_RECNO = 2  };


//---- Begin of function CampaignEastWest::stage_6_create_game ----//
//
// Vars defined in event functions before calling this function:
//
// -cur_monster_nation_recno
// -attack_state_recno
// -target_state_recno
//
int CampaignEastWest::stage_6_create_game()
{
	init_random_plot(0);		// no plot

	//---- set situation vars -----//

	set_std_situation();

	cur_situation.population_level = 80 + misc.random(20);
	cur_situation.town_loyalty_level = 80 + misc.random(20);
	cur_situation.unit_loyalty_level = 80 + misc.random(20);
	cur_situation.unit_combat_level  = 20 + misc.random(10);

	//---- create game objects and vars ----//

	if( !stage_6_create_nation() )
		return 0;

	if( !stage_6_create_town() )
		return 0;

	if( !stage_6_create_firm() )
		return 0;

	//------ create monsters ---------//

	create_troop(STAGE_6_MONSTER_NATION_RECNO, campaign_difficulty*7, 40+campaign_difficulty*5 );

	//------ init stage 1 vars -------//

	stage_6_init_vars();

	//---- generate plants and other objects on the map -----//

	world.gen_rocks();		// generate mountains

	world.generate_map2();

	//------- create additional raw sites ----------//

	create_raw_site();

	//------- init relationship -------//

	stage_6_init_relation();

	// ----- create royal units ------//

	create_royal_units(CAMPAIGN_PLAYER_NATION_RECNO);

	//------ plot create game --------//

	(this->*plot_create_game_FP)();

	return 1;
}
//---- End of function CampaignEastWest::stage_6_create_game -----//


//---- Begin of function CampaignEastWest::stage_6_init_vars ----//

void CampaignEastWest::stage_6_init_vars()
{
	//----- init monster reinforcement settings -----//

	static short reinforceCountArray[] = { 8, 16, 30, 50, 80 };

	monster_reinforcement_count = reinforceCountArray[campaign_difficulty-1];		// this is the number of units, not the number of times of reinforcement

	next_monster_reinforcement_date = info.game_date + (MAX_CAMPAIGN_DIFFICULTY-campaign_difficulty+1) * 30;

	//----- set player nation vars -----//

	init_player_resource(70);

	init_nation_resource(STAGE_6_MONSTER_NATION_RECNO, 50);

	//---- reset hero recno -------//

	hero_unit_recno = 0;
}
//---- End of function CampaignEastWest::stage_6_init_vars -----//


//---- Begin of function CampaignEastWest::stage_6_prelude ----//
//
// Pick a state to attack.
//
void CampaignEastWest::stage_6_prelude()
{
	attack_state( attacker_state_recno, target_state_recno, 0, 1, 2 );

	//------- set config settings -------//

	config.explore_whole_map = 1;
}
//---- End of function CampaignEastWest::stage_6_prelude -----//


//---- Begin of function CampaignEastWest::stage_6_create_nation ----//

int CampaignEastWest::stage_6_create_nation()
{
	//-------- create the player nation ------//

	state_array[target_state_recno]->add_game_nation();

	//------- create monster nation --------//

	state_array[attacker_state_recno]->add_game_nation();

	return 1;
}
//---- End of function CampaignEastWest::stage_6_create_nation ----//


//---- Begin of function CampaignEastWest::stage_6_create_town ----//

int CampaignEastWest::stage_6_create_town()
{
	//------- set the range of placements of objects -------//

	enum { TOWN_STATE_MAP_WIDTH = 100,
			 TOWN_STATE_MAP_HEIGHT = 100 };

	//------ generate a StateArray ---------//

	int townAddCount  = 5 + misc.random(2);		// add 5 to 6 player towns
	int addedCount=0;

	StateArray townStateArray;

	townStateArray.init(TOWN_STATE_MAP_WIDTH, TOWN_STATE_MAP_HEIGHT);
	townStateArray.generate(13);

	for( int i=townStateArray.size() ; i>0 && addedCount<townAddCount ; i-- )
	{
		StateInfo* stateInfo = townStateArray[i];

		int xLoc = (int) stateInfo->center_x * MAX_WORLD_X_LOC / TOWN_STATE_MAP_WIDTH;
		int yLoc = (int) stateInfo->center_y * MAX_WORLD_Y_LOC / TOWN_STATE_MAP_HEIGHT;

		int soldierAddCount = MAX_SOLDIER/4 + misc.random(MAX_SOLDIER/4) + 1;

		if( create_nation_town(PLAYER_NATION_RECNO, xLoc, yLoc, 1, 0, soldierAddCount) )		// 0-the race of the town need not to be the same as the nation
			addedCount++;
	}

	town_array.update_all_camp_link();

	return 1;
}
//---- End of function CampaignEastWest::stage_6_create_town ----//


//---- Begin of function CampaignEastWest::stage_6_create_firm ----//

int CampaignEastWest::stage_6_create_firm()
{
	//----- create firms around established towns -----//

	Nation* nationPtr = nation_array[PLAYER_NATION_RECNO];

	int townRecno = king_oversee_town_recno(PLAYER_NATION_RECNO);

	if( !create_economic_firm(townRecno) )
		return 0;

	return 1;
}
//---- End of function CampaignEastWest::stage_6_create_firm ----//


//------- Begin of function CampaignEastWest::stage_6_init_relation -------//
//
void CampaignEastWest::stage_6_init_relation()
{
	set_nation_hostile(PLAYER_NATION_RECNO, STAGE_6_MONSTER_NATION_RECNO);
}
//------- End of function CampaignEastWest::stage_6_init_relation --------//


//=================================================================//


//------- Begin of function CampaignEastWest::stage_6_next_day -------//
//
void CampaignEastWest::stage_6_next_day()
{
	(this->*plot_next_day_FP)();

	if( !nation_array.is_deleted(STAGE_6_MONSTER_NATION_RECNO) )
	{
		think_enemy_attack(STAGE_6_MONSTER_NATION_RECNO, PLAYER_NATION_RECNO);

		monster_reinforcement_to_lair();

		stage_6_create_hero();

		//---- check if the hero is killed ----//

		if( hero_unit_recno )
		{
			if( unit_array.is_deleted(hero_unit_recno) )
			{
				hero_unit_recno = 0;
				return;
			}

			Unit* heroUnit = unit_array[hero_unit_recno];

			if( heroUnit->is_civilian() ||
				 heroUnit->nation_recno != PLAYER_NATION_RECNO )
			{
				hero_unit_recno = 0;
				return;
			}
		}
	}
}
//------- End of function CampaignEastWest::stage_6_next_day --------//


//------- Begin of function CampaignEastWest::stage_6_create_hero -------//
//
void CampaignEastWest::stage_6_create_hero()
{
	if( hero_unit_recno )		// a hero has already been created
		return;

	//-------- locate a space for the hero ----------//

	int xLoc, yLoc;

	random_edge_location(xLoc, yLoc);

	world.check_unit_space_avoid_mountain = 1;	// set this for world.locate_space()_

	if( !world.locate_space( xLoc, yLoc, xLoc, yLoc, 2 , 2 ) )		// locate a space where a new unit can be created
	{
		world.check_unit_space_avoid_mountain = 0;
		return;
	}

	world.check_unit_space_avoid_mountain = 0;

	//-------- create the hero ----------//

	hero_unit_recno = hero_res.create_hero(xLoc, yLoc, 0,
							(~nation_array)->race_id, 80, 80+misc.random(20), -1, 0, 1 );		// last 0-the hero must be a fictional one as opposite to a historic one.	Last 1-use reserve

	if( hero_unit_recno )
	{
		Unit* unitPtr = unit_array[hero_unit_recno];

		unitPtr->item.set( ITEM_LEADER, 0 );

		unitPtr->skill.set_max_skill_level( 300 );
		unitPtr->skill.set_skill_level( 200 + misc.random(100) );

		unitPtr->skill.set_max_combat_level( 300 );
		unitPtr->set_combat_level( 200 + misc.random(100) );

		unitPtr->betray(PLAYER_NATION_RECNO);

		game.add_lose_condition( E_KILL_UNIT, 1, hero_unit_recno, 0 );	// 1-this condition only applies to the player.
	}
}
//------- End of function CampaignEastWest::stage_6_create_hero --------//


//============================================================//


//--- Begin of function CampaignEastWest::stage_6_process_game_result ---//
//
void CampaignEastWest::stage_6_process_game_result()
{
	//--- if the player won the game -----//

	if( game.result_win_condition_id >= 0 )
	{

	#ifdef DEBUG
		if( auto_test_flag )
		{
			set_event( EVENT_GIVE_THRONE );
			return;
		}
	#endif

		if( !unit_array.is_deleted(hero_unit_recno) )
		{
			Nation* ownNation = (~nation_array);

			// ######## patch begin 17/12 ########//
			if( unit_array.is_deleted(ownNation->king_unit_recno) )
			{
				old_king_name[0] = '\0';
				ownNation->set_king(hero_unit_recno, 0);		// 0 - this is not the first king
			}
			else if( ownNation->king_unit_recno != hero_unit_recno )
			{
				Unit* oldKingUnit = unit_array[ ownNation->king_unit_recno ];

				//--- save the name of the old king for displaying letter later ---//

				strncpy( old_king_name, oldKingUnit->unit_name(0), CUSTOM_NAME_LEN );		//0-without title
				old_king_name[CUSTOM_NAME_LEN] = '\0';

				//-- resign the old king and promote the hero as king ----//

				oldKingUnit->resign(COMMAND_AUTO);
				ownNation->set_king(hero_unit_recno, 0);		// 0 - this is not the first king
			}

			set_event( EVENT_GIVE_THRONE );
			// ######## patch end 17/12 ########//
		}
		else
		{
			set_event( EVENT_RANDOM_POINT_2 );
		}
	}

	//---- if the player lost the game ----//

	else
	{
		// must retry

		info.game_year = game_year;			// revert info.game_year back to campaign's game year so that the game's time will not advance

		misc.set_random_seed( saved_random_seed );		// restore the random seed so that the game will replay in the same way
	}
}
//---- End of function CampaignEastWest::stage_6_process_game_result -----//


//------ Begin of function CampaignEastWest::stage_6_disp_end_game_msg ------//
//
void CampaignEastWest::stage_6_disp_end_game_msg()
{
	if( game.result_win_condition_id >= 0 )
	{
		disp_in_game_msg( res_stage.read("6WIN") );
	}
	else
	{
		disp_in_game_msg( res_stage.read("6LOSE") );
	}
}
//------- End of function CampaignEastWest::stage_6_disp_end_game_msg -------//

