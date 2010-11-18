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

//Filename   : OC_EAS10.CPP
//Description: CampaignEastWest - STAGE_OPPORTUNISTIC_ATTACK

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

enum { STAGE_10_MONSTER_NATION_RECNO = 2  };


//---- Begin of function CampaignEastWest::stage_10_create_game ----//
//
// Vars defined in event functions before calling this function:
//
// -cur_monster_nation_recno
// -attack_state_recno
// -target_state_recno
//
int CampaignEastWest::stage_10_create_game()
{
	init_random_plot('A');

	plot_enemy_nation_recno = STAGE_10_MONSTER_NATION_RECNO;

	//---- create game objects and vars ----//

	if( !stage_10_create_nation() )
		return 0;

	if( !stage_10_create_lair() )
		return 0;

	//---- create the player force ----//

	create_troop(PLAYER_NATION_RECNO, 65-campaign_difficulty*5, 75 );		// 70 - average combat level, an elite force

	//------ init stage 1 vars -------//

	stage_10_init_vars();

	//---- generate plants and other objects on the map -----//

	world.gen_rocks();		// generate mountains

	world.generate_map2();

	//------- create additional raw sites ----------//

	create_raw_site();

	//------- init relationship -------//

	stage_10_init_relation();

	// ####### begin Gilbert 9/4 ########//
	// ----- create royal units ------//

	create_royal_units(CAMPAIGN_PLAYER_NATION_RECNO);
	// ####### end Gilbert 9/4 ########//

	//------ plot create game --------//

	(this->*plot_create_game_FP)();

	return 1;
}
//---- End of function CampaignEastWest::stage_10_create_game -----//


//---- Begin of function CampaignEastWest::stage_10_init_vars ----//

void CampaignEastWest::stage_10_init_vars()
{
	monster_reinforcement_count = 1;

	//----- set player nation vars -----//

	init_player_resource(40);

	init_nation_resource(STAGE_10_MONSTER_NATION_RECNO, 40);
}
//---- End of function CampaignEastWest::stage_10_init_vars -----//


//---- Begin of function CampaignEastWest::stage_10_prelude ----//
//
// Pick a state to attack.
//
void CampaignEastWest::stage_10_prelude()
{
	play_speech("PRE-10.WAV");
	disp_narrative( res_stage.read("10PRELUD") );
	stop_speech();

	attack_state( attacker_state_recno, target_state_recno, 0, 1, 2 );

	//------- set config settings -------//

	config.explore_whole_map = 0;
}
//---- End of function CampaignEastWest::stage_10_prelude -----//


//---- Begin of function CampaignEastWest::stage_10_create_nation ----//

int CampaignEastWest::stage_10_create_nation()
{
	//------- create player nation --------//

	state_array[attacker_state_recno]->add_game_nation();

	//-------- create the monster nation ------//

	state_array[target_state_recno]->add_game_nation();

	return 1;
}
//---- End of function CampaignEastWest::stage_10_create_nation ----//


//---- Begin of function CampaignEastWest::stage_10_create_lair ----//

int CampaignEastWest::stage_10_create_lair()
{
	int lairAddCount = 5 + m.random(2);		// add 5 to 6 player towns
	int independentTownAddCount = 0;
	int specialFirmCount = 0;

	create_lair(cur_monster_nation_recno, STAGE_10_MONSTER_NATION_RECNO, lairAddCount, independentTownAddCount, specialFirmCount);

	return 1;
}
//---- End of function CampaignEastWest::stage_10_create_lair ----//


//------- Begin of function CampaignEastWest::stage_10_init_relation -------//
//
void CampaignEastWest::stage_10_init_relation()
{
	set_nation_hostile(PLAYER_NATION_RECNO, STAGE_10_MONSTER_NATION_RECNO);
}
//------- End of function CampaignEastWest::stage_10_init_relation --------//


//=================================================================//


//------- Begin of function CampaignEastWest::stage_10_next_day -------//
//
void CampaignEastWest::stage_10_next_day()
{
	//------- call plot next day --------//

	(this->*plot_next_day_FP)();

	//-----------------------------------//

	if( !nation_array.is_deleted(STAGE_10_MONSTER_NATION_RECNO) )
	{
		stage_10_monster_reinforcement();

		think_enemy_attack(STAGE_10_MONSTER_NATION_RECNO, PLAYER_NATION_RECNO);
   }
}
//------- End of function CampaignEastWest::stage_10_next_day --------//


//--- Begin of function CampaignEastWest::stage_10_monster_reinforcement ---//
//
void CampaignEastWest::stage_10_monster_reinforcement()
{
	if( monster_reinforcement_count==0 )		// only call reinforcement once.
		return;

	if( info.game_date%5 > 0 )		// check once every 5 days
		return;

	//--- when the Fryhtans are almost destroyed, Fryhtan reinforcements come ---//

	nation_array.update_military_rating();

	if( nation_array[STAGE_10_MONSTER_NATION_RECNO]->military_rating > 50 )
		return;

	//--------- create monster reinforcement now ----------//

	create_troop(STAGE_10_MONSTER_NATION_RECNO, 10+campaign_difficulty*10);

	monster_reinforcement_count = 0;
}
//--- End of function CampaignEastWest::stage_10_monster_reinforcement ----//


//============================================================//


//--- Begin of function CampaignEastWest::stage_10_process_game_result ---//
//
void CampaignEastWest::stage_10_process_game_result()
{
	//--- if the player won the game -----//

	if( game.result_win_condition_id >= 0 )
	{
		attack_state( attacker_state_recno, target_state_recno, 1, 2, 3 );

		set_event( EVENT_RANDOM_POINT_2 );
	}

	//---- if the player lost the game ----//

	else
	{
		// must retry

		info.game_year = game_year;			// revert info.game_year back to campaign's game year so that the game's time will not advance

		m.set_random_seed( saved_random_seed );		// restore the random seed so that the game will replay in the same way
	}
}
//---- End of function CampaignEastWest::stage_10_process_game_result -----//


//------ Begin of function CampaignEastWest::stage_10_disp_end_game_msg ------//
//
void CampaignEastWest::stage_10_disp_end_game_msg()
{
	if( game.result_win_condition_id >= 0 )
	{
		disp_in_game_msg( res_stage.read("10WIN") );
	}
	else
	{
		disp_in_game_msg( res_stage.read("10LOSE") );
	}
}
//------- End of function CampaignEastWest::stage_10_disp_end_game_msg -------//

