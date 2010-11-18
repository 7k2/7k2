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

//Filename   : OC_EAS15.CPP
//Description: CampaignEastWest - STAGE_ACQUIRE_MEGA_WEAPON

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

enum { STAGE_15_MONSTER_NATION_RECNO = 2  };


//---- Begin of function CampaignEastWest::stage_15_create_game ----//
//
// Vars defined in event functions before calling this function:
//
// -cur_monster_nation_recno
// -attack_state_recno
// -target_state_recno
//
int CampaignEastWest::stage_15_create_game()
{
	// ###### begin Gilbert 9/4 ########//
	init_random_plot(0);		// no plot
	// ###### end Gilbert 9/4 ########//

	//---- create game objects and vars ----//

	if( !stage_15_create_nation() )
		return 0;

	if( !stage_15_create_lair() )
		return 0;

	//---- create the player force ----//

	create_troop(PLAYER_NATION_RECNO, 44-campaign_difficulty*4 );

	//------ init stage 1 vars -------//

	stage_15_init_vars();

	//---- generate plants and other objects on the map -----//

	world.gen_rocks();		// generate mountains

	world.generate_map2();

	//------- create additional raw sites ----------//

	create_raw_site();

	//------- init relationship -------//

	stage_15_init_relation();

	// ####### begin Gilbert 9/4 ########//
	// ----- create royal units ------//

	create_royal_units(CAMPAIGN_PLAYER_NATION_RECNO);

	//------ plot create game --------//

	(this->*plot_create_game_FP)();
	// ####### end Gilbert 9/4 ########//

	return 1;
}
//---- End of function CampaignEastWest::stage_15_create_game -----//


//---- Begin of function CampaignEastWest::stage_15_init_vars ----//

void CampaignEastWest::stage_15_init_vars()
{
	//--- set the Fryhtan All-high to own a mega-weapon blueprint ---//

	short techIdArray[] = { TECH_MEGA_CANNON,	TECH_MEGA_F_BALLISTA, TECH_MEGA_SAW };

	mega_tech_id = techIdArray[m.random(3)];

	tech_res[mega_tech_id]->set_nation_tech_level(STAGE_15_MONSTER_NATION_RECNO, 1);

	//----- set player nation vars -----//

	init_player_resource(50);

	init_nation_resource(STAGE_15_MONSTER_NATION_RECNO, 60);

	//-------- add victory conditions -------//

	game.reset_win_condition();		// so that only achieving E_ACQUIRE_TECH will win the game.

	game.add_win_condition( E_ACQUIRE_TECH, 1, mega_tech_id );
}
//---- End of function CampaignEastWest::stage_15_init_vars -----//


//---- Begin of function CampaignEastWest::stage_15_prelude ----//
//
// Pick a state to attack.
//
void CampaignEastWest::stage_15_prelude()
{
	attack_state( attacker_state_recno, target_state_recno, 0, 1, 2 );

	//------- set config settings -------//

	config.explore_whole_map = 0;

   config.terrain_set = 2;		// Badlands terrain set 
}
//---- End of function CampaignEastWest::stage_15_prelude -----//


//---- Begin of function CampaignEastWest::stage_15_create_nation ----//

int CampaignEastWest::stage_15_create_nation()
{
	//------- create player nation --------//

	state_array[attacker_state_recno]->add_game_nation();

	//-------- create the monster nation ------//

	state_array[target_state_recno]->add_game_nation();

	return 1;
}
//---- End of function CampaignEastWest::stage_15_create_nation ----//


//---- Begin of function CampaignEastWest::stage_15_create_lair ----//

int CampaignEastWest::stage_15_create_lair()
{
	int lairAddCount = 6 + m.random(2);						// add 6 to 7 Fryhtan Lairs
	int independentTownAddCount=0;
	int specialFirmCount= 4 + m.random(2);

	if( m.random(2)==0 )		// 50% chance having independent towns
		independentTownAddCount = 6 + m.random(3);		// 6 to 8 independent towns

	create_lair(cur_monster_nation_recno, STAGE_15_MONSTER_NATION_RECNO, lairAddCount, independentTownAddCount, specialFirmCount);

	return 1;
}
//---- End of function CampaignEastWest::stage_15_create_lair ----//


//------- Begin of function CampaignEastWest::stage_15_init_relation -------//
//
void CampaignEastWest::stage_15_init_relation()
{
	set_nation_hostile(PLAYER_NATION_RECNO, STAGE_15_MONSTER_NATION_RECNO);
}
//------- End of function CampaignEastWest::stage_15_init_relation --------//


//=================================================================//


//------- Begin of function CampaignEastWest::stage_15_next_day -------//
//
void CampaignEastWest::stage_15_next_day()
{
	//--- set the tech level here, as the setting will be overiden in fetch_nation_info ---//

	if( !nation_array.is_deleted(STAGE_15_MONSTER_NATION_RECNO) )
		tech_res[mega_tech_id]->set_nation_tech_level(STAGE_15_MONSTER_NATION_RECNO, 1);

	(this->*plot_next_day_FP)();
}
//------- End of function CampaignEastWest::stage_15_next_day --------//


//============================================================//


//--- Begin of function CampaignEastWest::stage_15_process_game_result ---//
//
void CampaignEastWest::stage_15_process_game_result()
{
	//--- if the player won the game -----//

	if( game.result_win_condition_id == E_ACQUIRE_TECH )
	{
		//----- randomly pick attacking and target states -----//

		if( player_pick_attack_target(CAMPAIGN_NATION_MONSTER) )
		{
			set_stage( STAGE_USE_MEGA_WEAPON );
		}
		else
		{
			set_event( EVENT_RANDOM_POINT_3 );
		}
	}

	//---- if the player lost the game ----//

	else
	{
		// must retry

		info.game_year = game_year;			// revert info.game_year back to campaign's game year so that the game's time will not advance

		m.set_random_seed( saved_random_seed );		// restore the random seed so that the game will replay in the same way
	}
}
//---- End of function CampaignEastWest::stage_15_process_game_result -----//


//------ Begin of function CampaignEastWest::stage_15_disp_end_game_msg ------//
//
void CampaignEastWest::stage_15_disp_end_game_msg()
{
	if( game.result_win_condition_id == E_ACQUIRE_TECH )
	{
		disp_in_game_msg( res_stage.read("15WIN") );
	}
	else
	{
		disp_in_game_msg( res_stage.read("15LOSE") );
	}
}
//------- End of function CampaignEastWest::stage_15_disp_end_game_msg -------//

