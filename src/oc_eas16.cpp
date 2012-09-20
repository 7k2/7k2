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

//Filename   : OC_EAST16.CPP
//Description: CampaignEastWest - STAGE_USE_MEGA_WEAPON

#include <all.h>
#include <osite.h>
#include <otech.h>
#include <ogame.h>
#include <otechres.h>
#include <ostate.h>
#include <oraceres.h>
#include <onews.h>
#include <omonsres.h>
#include <onation2.h>
#include <oc_east.h>
#include <of_camp.h>


//----- define constant ----------//

enum { STAGE_16_MONSTER_NATION_RECNO = 2  };


//---- Begin of function CampaignEastWest::stage_16_create_game ----//
//
// Vars defined in event functions before calling this function:
//
// -cur_monster_nation_recno
// -attack_state_recno
// -target_state_recno
//
int CampaignEastWest::stage_16_create_game()
{
	// ###### begin Gilbert 9/4 ########//
	init_random_plot(0);		// no plot
	// ###### end Gilbert 9/4 ########//

	//---- create game objects and vars ----//

	if( !stage_16_create_nation() )
		return 0;

	if( !stage_16_create_lair() )
		return 0;

	//---- create a troop and a group of mega-weapons ----//

	create_troop(PLAYER_NATION_RECNO, 46-campaign_difficulty*6 );		// 80 - average combat level, an elite force

	create_troop(PLAYER_NATION_RECNO, 13-campaign_difficulty, 0, tech_res[mega_tech_id]->unit_id, 1 );		// 1-use last location

	//------ init stage 1 vars -------//

	stage_16_init_vars();

	//---- generate plants and other objects on the map -----//

	world.gen_rocks();		// generate mountains

	world.generate_map2();

	//------- create additional raw sites ----------//

	create_raw_site();

	//------- init relationship -------//

	stage_16_init_relation();

	// ####### begin Gilbert 9/4 ########//
	// ----- create royal units ------//

	create_royal_units(CAMPAIGN_PLAYER_NATION_RECNO);

	//------ plot create game --------//

	(this->*plot_create_game_FP)();
	// ####### end Gilbert 9/4 ########//

	return 1;
}
//---- End of function CampaignEastWest::stage_16_create_game -----//


//---- Begin of function CampaignEastWest::stage_16_init_vars ----//

void CampaignEastWest::stage_16_init_vars()
{
	//----- set player nation vars -----//

	init_player_resource(70);

	init_nation_resource(STAGE_16_MONSTER_NATION_RECNO, 50);
}
//---- End of function CampaignEastWest::stage_16_init_vars -----//


//---- Begin of function CampaignEastWest::stage_16_prelude ----//
//
// Pick a state to attack.
//
void CampaignEastWest::stage_16_prelude()
{
	attack_state( attacker_state_recno, target_state_recno, 0, 1, 2 );

	//------- set config settings -------//

	config.explore_whole_map = 0;

   config.terrain_set = 2;		// Badlands terrain set 
}
//---- End of function CampaignEastWest::stage_16_prelude -----//


//---- Begin of function CampaignEastWest::stage_16_create_nation ----//

int CampaignEastWest::stage_16_create_nation()
{
	//------- create player nation --------//

	state_array[attacker_state_recno]->add_game_nation();

	//-------- create the monster nation ------//

	state_array[target_state_recno]->add_game_nation();

	return 1;
}
//---- End of function CampaignEastWest::stage_16_create_nation ----//


//---- Begin of function CampaignEastWest::stage_16_create_lair ----//

int CampaignEastWest::stage_16_create_lair()
{
	int lairAddCount = 4 + campaign_difficulty + misc.random(2);						// add 6 to 7 Fryhtan Lairs
	int independentTownAddCount=0;
	int specialFirmCount= 2 + misc.random(2) + campaign_difficulty/2;
	int soldierCombatLevel = 45 + campaign_difficulty*5;

	if( misc.random(2)==0 )		// 50% chance having independent towns
		independentTownAddCount = 6 + misc.random(3);		// 6 to 8 independent towns

	create_lair(cur_monster_nation_recno, STAGE_16_MONSTER_NATION_RECNO, lairAddCount, independentTownAddCount, specialFirmCount, soldierCombatLevel);

	return 1;
}
//---- End of function CampaignEastWest::stage_16_create_lair ----//


//------- Begin of function CampaignEastWest::stage_16_init_relation -------//
//
void CampaignEastWest::stage_16_init_relation()
{
	set_nation_hostile(PLAYER_NATION_RECNO, STAGE_16_MONSTER_NATION_RECNO);
}
//------- End of function CampaignEastWest::stage_16_init_relation --------//


//=================================================================//


//------- Begin of function CampaignEastWest::stage_16_next_day -------//
//
void CampaignEastWest::stage_16_next_day()
{
	// ###### begin Gilbert 9/4 ########//
	(this->*plot_next_day_FP)();
	// ###### end Gilbert 9/4 ########//
}
//------- End of function CampaignEastWest::stage_16_next_day --------//


//============================================================//


//--- Begin of function CampaignEastWest::stage_16_process_game_result ---//
//
void CampaignEastWest::stage_16_process_game_result()
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
		// must retry

		info.game_year = game_year;			// revert info.game_year back to campaign's game year so that the game's time will not advance

		misc.set_random_seed( saved_random_seed );		// restore the random seed so that the game will replay in the same way
	}
}
//---- End of function CampaignEastWest::stage_16_process_game_result -----//


//------ Begin of function CampaignEastWest::stage_16_disp_end_game_msg ------//
//
void CampaignEastWest::stage_16_disp_end_game_msg()
{
	if( game.result_win_condition_id >= 0 )
	{
		disp_in_game_msg( res_stage.read("16WIN") );
	}
	else
	{
		disp_in_game_msg( res_stage.read("16LOSE") );
	}
}
//------- End of function CampaignEastWest::stage_16_disp_end_game_msg -------//

