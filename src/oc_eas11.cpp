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

//Filename   : OC_EAS11.CPP
//Description: CampaignEastWest - STAGE_FRYHTAN_JOINT_ATTACK

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

enum { STAGE_11_ALLIED_MONSTER_NATION_RECNO = 2,
		 STAGE_11_ENEMY_MONSTER_NATION_RECNO = 3   };

//---- Begin of function CampaignEastWest::stage_11_create_game ----//
//
// Vars defined in event functions before calling this function:
//
// -cur_monster_nation_recno    - the attacking monster nation recno
// -target_monster_nation_recno - the target monster nation recno
// -attacker_state_recno	     - the monster attacking state
// -attacker_state_recno2 		  - the player attacking state
// -target_state_recno			  - the monster target state
//
int CampaignEastWest::stage_11_create_game()
{
	init_random_plot('A');

	plot_enemy_nation_recno = STAGE_11_ENEMY_MONSTER_NATION_RECNO;

	//---- create the defending monster objects ----//

	if( !stage_11_create_nation() )
		return 0;

	//------- init relationship -------//

	stage_11_init_relation();		// must init relationship first so that allied units are visible on the map

	//------- create lairs ----------//

	if( !stage_11_create_lair() )
		return 0;

	//---- create the attacking monster force ----//

	create_troop(STAGE_11_ALLIED_MONSTER_NATION_RECNO, 55-campaign_difficulty*5, 50+misc.random(20) );		// 50-70 combat level

	//---- create the attacking player force ----//

	create_troop(PLAYER_NATION_RECNO, 55-campaign_difficulty*5, 60+misc.random(10) );

	//------ init stage 1 vars -------//

	stage_11_init_vars();

	//---- generate plants and other objects on the map -----//

	world.gen_rocks();		// generate mountains

	world.generate_map2();

	//------- create additional raw sites ----------//

	create_raw_site();

	//----- create royal units ------//

	create_royal_units(CAMPAIGN_PLAYER_NATION_RECNO);

	//------ plot create game --------//

	(this->*plot_create_game_FP)();

	return 1;
}
//---- End of function CampaignEastWest::stage_11_create_game -----//


//---- Begin of function CampaignEastWest::stage_11_init_vars ----//

void CampaignEastWest::stage_11_init_vars()
{
	//----- set player nation vars -----//

	init_player_resource(40);

	init_nation_resource(STAGE_11_ALLIED_MONSTER_NATION_RECNO, 40);

	init_nation_resource(STAGE_11_ENEMY_MONSTER_NATION_RECNO, 70);

	//-------- add victory conditions -------//

	game.add_win_condition( E_DEFEAT_KINGDOM, 1, STAGE_11_ENEMY_MONSTER_NATION_RECNO );  	// the objective is to destroy the enemy kingdom
}
//---- End of function CampaignEastWest::stage_11_init_vars -----//


//---- Begin of function CampaignEastWest::stage_11_prelude ----//
//
// Pick a state to attack.
//
// cur_monster_nation_recno is the attacking Fryhtan nation
// target_monster_nation_recno is the target Fryhtan nation
//
void CampaignEastWest::stage_11_prelude()
{
	play_speech("PRE-11.WAV");
	disp_narrative( res_stage.read("11PRELUD") );
	stop_speech();

	attack_state( attacker_state_recno, target_state_recno, 0, 1, 2, attacker2_state_recno );

	//------- set config settings -------//

	config.explore_whole_map = 0;
}
//---- End of function CampaignEastWest::stage_11_prelude -----//


//---- Begin of function CampaignEastWest::stage_11_create_nation ----//

int CampaignEastWest::stage_11_create_nation()
{
	//------- create player nation --------//

	state_array[attacker2_state_recno]->add_game_nation();

	//------- create the attacking monster nation --------//

	state_array[attacker_state_recno]->add_game_nation();

	//-------- create the target monster nation ------//

	state_array[target_state_recno]->add_game_nation();

	return 1;
}
//---- End of function CampaignEastWest::stage_11_create_nation ----//


//---- Begin of function CampaignEastWest::stage_11_create_lair ----//

int CampaignEastWest::stage_11_create_lair()
{
	int lairAddCount = 8 + misc.random(2);						// add 6 to 7 Fryhtan Lairs
	int specialFirmCount = 5 + misc.random(2);
	int independentTownAddCount = 0;
	int combatLevel = 54 + campaign_difficulty*6;

	create_lair(cur_monster_nation_recno, STAGE_11_ENEMY_MONSTER_NATION_RECNO, lairAddCount, independentTownAddCount, specialFirmCount, combatLevel);

	return 1;
}
//---- End of function CampaignEastWest::stage_11_create_lair ----//


//------- Begin of function CampaignEastWest::stage_11_init_relation -------//
//
void CampaignEastWest::stage_11_init_relation()
{
	set_nation_allied (PLAYER_NATION_RECNO, STAGE_11_ALLIED_MONSTER_NATION_RECNO);
	set_nation_hostile(PLAYER_NATION_RECNO, STAGE_11_ENEMY_MONSTER_NATION_RECNO );
}
//------- End of function CampaignEastWest::stage_11_init_relation --------//


//=================================================================//


//------- Begin of function CampaignEastWest::stage_11_next_day -------//
//
void CampaignEastWest::stage_11_next_day()
{
	//------- call plot next day --------//

	(this->*plot_next_day_FP)();
}
//------- End of function CampaignEastWest::stage_11_next_day --------//


//============================================================//


//--- Begin of function CampaignEastWest::stage_11_process_game_result ---//
//
void CampaignEastWest::stage_11_process_game_result()
{
	//--- if the player won the game -----//

	if( game.result_win_condition_id >= 0 )
	{
		attack_state( attacker_state_recno, target_state_recno, 1, 2, 3, attacker2_state_recno );

		//-- if the attacking monster is still alive and you are still allied with it --//

		if( fryhtan_joint_attack_reward &&
			 !nation_array.is_deleted(STAGE_11_ALLIED_MONSTER_NATION_RECNO) )
		{
			if( state_array.nation_state_count(cur_monster_nation_recno) > 1 &&			// if the allied monster only has one state, it will not give land
				 (~nation_array)->get_relation_status(STAGE_11_ALLIED_MONSTER_NATION_RECNO) == RELATION_ALLIANCE &&
				 misc.random(10) < 7 ) 		// 70% chance that the allied Fryhtan will deliver its promise
			{
				//---- the allied Frythan gives a state to the player ----//

				int rc = random_pick_attack_state(western_nation_recno, cur_monster_nation_recno);

				if( !rc )		// if the monster doesn't have any state next to the player nation, then pick any state
					target_state_recno = random_pick_state(cur_monster_nation_recno);

				if( target_state_recno )
					state_change_nation( target_state_recno, western_nation_recno );

				//--- the Fryhtan agrees to give land ---//

// 			disp_narrative( res_stage.read("11POST1") );   //BUGHERE

				//--- set the relation with the attacking fryhtan kingdom back to neutral after the battle ----//

				(~nation_array)->set_relation_status(STAGE_11_ALLIED_MONSTER_NATION_RECNO, RELATION_NEUTRAL);
			}
			else
			{
				//-- the Fryhtan agrees to deliver its promise of giving land --//

//				disp_narrative( res_stage.read("11POST2") );   //BUGHERE
			}
		}

		set_event( EVENT_RANDOM_POINT_2 );
	}

	//---- if the player lost the game ----//

	else
	{
		// must retry

		info.game_year = game_year;			// revert info.game_year back to campaign's game year so that the game's time will not advance

		misc.set_random_seed( saved_random_seed );		// restore the random seed so that the game will replay in the same way
	}
}
//---- End of function CampaignEastWest::stage_11_process_game_result -----//


//------ Begin of function CampaignEastWest::stage_11_disp_end_game_msg ------//
//
void CampaignEastWest::stage_11_disp_end_game_msg()
{
	if( game.result_win_condition_id >= 0 )
	{
		disp_in_game_msg( res_stage.read("11WIN") );
	}
	else
	{
		disp_in_game_msg( res_stage.read("11LOSE") );
	}
}
//------- End of function CampaignEastWest::stage_11_disp_end_game_msg -------//

