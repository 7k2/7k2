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

//Filename   : OC_EAST14.CPP
//Description: CampaignEastWest - STAGE_DEEP_INTO_FRYHTAN_LAND

#include <ALL.H>
#include <OSITE.H>
#include <OTECH.H>
#include <OGAME.H>
#include <OTECHRES.H>
#include <OSTATE.H>
#include <ORACERES.H>
#include <ONEWS.H>
#include <OMONSRES.H>
#include <ONATION2.H>
#include <OC_EAST.H>
#include <OF_CAMP.H>


//----- define constant ----------//

enum { STAGE_14_MONSTER_NATION_RECNO = 2  };


//---- Begin of function CampaignEastWest::stage_14_create_game ----//
//
// Vars defined in event functions before calling this function:
//
// -cur_monster_nation_recno
// -attack_state_recno
// -target_state_recno
//
int CampaignEastWest::stage_14_create_game()
{
	init_random_plot('A');

	plot_enemy_nation_recno = STAGE_14_MONSTER_NATION_RECNO;

	//---- create game objects and vars ----//

	if( !stage_14_create_nation() )
		return 0;

	if( !stage_14_create_lair() )
		return 0;

	//---- create the player force ----//

	create_troop(PLAYER_NATION_RECNO, 66-campaign_difficulty*6, 50+m.random(20) );		// 80 - average combat level, an elite force

	//------ init stage 1 vars -------//

	stage_14_init_vars();

	//---- generate plants and other objects on the map -----//

	world.gen_rocks();		// generate mountains

	world.generate_map2();

	//------- create additional raw sites ----------//

	create_raw_site();

	//------- init relationship -------//

	stage_14_init_relation();

	// ####### begin Gilbert 9/4 ########//
	// ----- create royal units ------//

	create_royal_units(CAMPAIGN_PLAYER_NATION_RECNO);
	// ####### end Gilbert 9/4 ########//
	//------ plot create game --------//

	(this->*plot_create_game_FP)();

	return 1;
}
//---- End of function CampaignEastWest::stage_14_create_game -----//


//---- Begin of function CampaignEastWest::stage_14_init_vars ----//

void CampaignEastWest::stage_14_init_vars()
{
	//----- set player nation vars -----//

	init_player_resource(40);

	init_nation_resource(STAGE_14_MONSTER_NATION_RECNO, 50);
}
//---- End of function CampaignEastWest::stage_14_init_vars -----//


//---- Begin of function CampaignEastWest::stage_14_prelude ----//
//
// Pick a state to attack.
//
void CampaignEastWest::stage_14_prelude()
{
//	disp_narrative( res_stage.read("14PRELUD") );

	attack_state( attacker_state_recno, target_state_recno, 0, 1, 2 );

	//------- set config settings -------//

	config.explore_whole_map = 0;

   config.terrain_set = 2;		// Badlands terrain set 
}
//---- End of function CampaignEastWest::stage_14_prelude -----//


//---- Begin of function CampaignEastWest::stage_14_create_nation ----//

int CampaignEastWest::stage_14_create_nation()
{
	//------- create player nation --------//

	state_array[attacker_state_recno]->add_game_nation();

	//-------- create the monster nation ------//

	state_array[target_state_recno]->add_game_nation();

	return 1;
}
//---- End of function CampaignEastWest::stage_14_create_nation ----//


//---- Begin of function CampaignEastWest::stage_14_create_lair ----//

int CampaignEastWest::stage_14_create_lair()
{
	int lairAddCount = 8 + m.random(3);						// add 6 to 7 Fryhtan Lairs
	int independentTownAddCount=0;
	int specialFirmCount= 5 + m.random(2);
	int soldierCombatLevel = 55 + campaign_difficulty * 5;

	if( m.random(2)==0 )		// 50% chance having independent towns
		independentTownAddCount = 6 + m.random(3);		// 6 to 8 independent towns

	create_lair(cur_monster_nation_recno, STAGE_14_MONSTER_NATION_RECNO, lairAddCount, independentTownAddCount, specialFirmCount, soldierCombatLevel);

	return 1;
}
//---- End of function CampaignEastWest::stage_14_create_lair ----//


//------- Begin of function CampaignEastWest::stage_14_init_relation -------//
//
void CampaignEastWest::stage_14_init_relation()
{
	set_nation_hostile(PLAYER_NATION_RECNO, STAGE_14_MONSTER_NATION_RECNO);
}
//------- End of function CampaignEastWest::stage_14_init_relation --------//


//=================================================================//


//------- Begin of function CampaignEastWest::stage_14_next_day -------//
//
void CampaignEastWest::stage_14_next_day()
{
	//------- call plot next day --------//

	(this->*plot_next_day_FP)();
}
//------- End of function CampaignEastWest::stage_14_next_day --------//


//============================================================//


//--- Begin of function CampaignEastWest::stage_14_process_game_result ---//
//
void CampaignEastWest::stage_14_process_game_result()
{
	//--- if the player won the game -----//

	if( game.result_win_condition_id >= 0 )
	{
		attack_state( attacker_state_recno, target_state_recno, 1, 2, 3 );

		set_event( EVENT_RANDOM_POINT_3 );
	}

	//---- if the player lost the game ----//

	else
	{
		cur_monster_nation_recno = random_pick_monster_campaign_nation();

		//----- randomly pick attacking and target states -----//

		if( random_pick_attack_state(western_nation_recno, cur_monster_nation_recno) &&
			 !has_stage_run(STAGE_ACQUIRE_MEGA_WEAPON) )
		{
			set_stage( STAGE_ACQUIRE_MEGA_WEAPON );
		}
		else
		{
			set_event( EVENT_RANDOM_POINT_3 );
		}
	}
}
//---- End of function CampaignEastWest::stage_14_process_game_result -----//


//------ Begin of function CampaignEastWest::stage_14_disp_end_game_msg ------//
//
void CampaignEastWest::stage_14_disp_end_game_msg()
{
	if( game.result_win_condition_id >= 0 )
	{
		disp_in_game_msg( res_stage.read("14WIN") );
	}
	else
	{
		disp_in_game_msg( res_stage.read("14LOSE") );
	}
}
//------- End of function CampaignEastWest::stage_14_disp_end_game_msg -------//

