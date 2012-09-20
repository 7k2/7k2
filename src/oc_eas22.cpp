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

//Filename   : OC_EAS22.CPP
//Description: CampaignEastWest - STAGE_ATTACK_NEW_KINGDOM

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

//------ Define constant ------//

#define STAGE_22_NEW_KINGDOM_NATION_RECNO	2


//---- Begin of function CampaignEastWest::stage_22_create_game ----//
//
// Vars defined in event functions before calling this function:
//
// -new_kingdom_nation_recno
// -attack_state_recno
// -target_state_recno
//
int CampaignEastWest::stage_22_create_game()
{
	// ###### begin Gilbert 1/4 ########//
	init_random_plot('C');
	// ###### end Gilbert 1/4 ########//

	if( !stage_22_create_nation() )
		return 0;

	if( !stage_22_create_town() )
		return 0;

	if( !stage_22_create_firm() )
		return 0;

	create_offensive_structure( STAGE_22_NEW_KINGDOM_NATION_RECNO, campaign_difficulty );

	//------ create the player troop -------//

	create_troop(PLAYER_NATION_RECNO, 55-campaign_difficulty*5);

	//------ init stage 1 vars -------//

	stage_22_init_vars();

	//---- generate plants and other objects on the map -----//

	world.gen_rocks();

	world.generate_map2();

	//------- create additional raw sites ----------//

	create_raw_site();

	//------- init relationship -------//

	stage_22_init_relation();

	// ####### begin Gilbert 9/4 ########//
	// ----- create royal units ------//

	create_royal_units(CAMPAIGN_PLAYER_NATION_RECNO);
	// ####### end Gilbert 9/4 ########//

	// ###### begin Gilbert 1/4 ########//
	(this->*plot_create_game_FP)();
	// ###### end Gilbert 1/4 ########//

	return 1;
}
//---- End of function CampaignEastWest::stage_22_create_game -----//


//---- Begin of function CampaignEastWest::stage_22_init_vars ----//

void CampaignEastWest::stage_22_init_vars()
{
	//------- set player nation vars -------//

	init_player_resource(40);

	init_nation_resource(STAGE_22_NEW_KINGDOM_NATION_RECNO, 60);
}
//---- End of function CampaignEastWest::stage_22_init_vars -----//


//---- Begin of function CampaignEastWest::stage_22_prelude ----//
//
// Pick a state to attack.
//
void CampaignEastWest::stage_22_prelude()
{
	play_speech("PRE-22.WAV");
	disp_narrative( res_stage.read("22PRELUD") );
	stop_speech();

	attack_state( attacker_state_recno, target_state_recno, 0, 1, 2 );

	//------- set config settings -------//

	config.explore_whole_map = 0;
}
//---- End of function CampaignEastWest::stage_22_prelude -----//


//---- Begin of function CampaignEastWest::stage_22_create_nation ----//

int CampaignEastWest::stage_22_create_nation()
{
	state_array[attacker_state_recno]->add_game_nation();
	state_array[target_state_recno]->add_game_nation();

	return 1;
}
//---- End of function CampaignEastWest::stage_22_create_nation ----//


//---- Begin of function CampaignEastWest::stage_22_create_town ----//

int CampaignEastWest::stage_22_create_town()
{
	int townAddCount   = 7 + misc.random(3);
	int hasFortPercent = 70;

	return create_town(STAGE_22_NEW_KINGDOM_NATION_RECNO, townAddCount, hasFortPercent);
}
//---- End of function CampaignEastWest::stage_22_create_town ----//


//---- Begin of function CampaignEastWest::stage_22_create_firm ----//

int CampaignEastWest::stage_22_create_firm()
{
	//----- create firms around established towns -----//

	Nation* nationPtr = nation_array[STAGE_22_NEW_KINGDOM_NATION_RECNO];

	int townRecno1 = king_oversee_town_recno(STAGE_22_NEW_KINGDOM_NATION_RECNO);
	int townRecno2 = random_pick_town_with_camp(STAGE_22_NEW_KINGDOM_NATION_RECNO, 2);		// only pick towns with <= 2 links

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

	create_economic_firm(townRecno1);

	if( townRecno2 )
		create_military_firm(townRecno2, 70);		// 70-power rating

	//---- create one more set of military structures -----//

	townRecno2 = random_pick_town_with_camp(STAGE_22_NEW_KINGDOM_NATION_RECNO, 2);		// only pick towns with <= 2 links

	if( townRecno2 )
		create_military_firm(townRecno2, 50);

	return 1;
}
//---- End of function CampaignEastWest::stage_22_create_firm ----//


//------- Begin of function CampaignEastWest::stage_22_init_relation -------//
//
void CampaignEastWest::stage_22_init_relation()
{
	set_nation_hostile(PLAYER_NATION_RECNO, STAGE_22_NEW_KINGDOM_NATION_RECNO);
}
//------- End of function CampaignEastWest::stage_22_init_relation --------//


//=================================================================//


//------- Begin of function CampaignEastWest::stage_22_next_day -------//
//
void CampaignEastWest::stage_22_next_day()
{
	// ###### begin Gilbert 1/4 ########//
	(this->*plot_next_day_FP)();
	// ###### end Gilbert 1/4 ########//
}
//------- End of function CampaignEastWest::stage_22_next_day --------//


//============================================================//


//--- Begin of function CampaignEastWest::stage_22_process_game_result ---//
//
void CampaignEastWest::stage_22_process_game_result()
{
	//--- if the player won the game -----//

	if( game.result_win_condition_id >= 0 )
	{
		attack_state( attacker_state_recno, target_state_recno, 1, 2, 3 );

		set_event( EVENT_RANDOM_POINT_4 );
	}

	//---- if the player lost the game ----//

	else
	{
		set_event( EVENT_RANDOM_POINT_4 );
	}
}
//---- End of function CampaignEastWest::stage_22_process_game_result -----//


//------ Begin of function CampaignEastWest::stage_22_disp_end_game_msg ------//
//
void CampaignEastWest::stage_22_disp_end_game_msg()
{
	if( game.result_win_condition_id >= 0 )
	{
		disp_in_game_msg( res_stage.read("22WIN") );
	}
	else
	{
		disp_in_game_msg( res_stage.read("22LOSE") );
	}
}
//------- End of function CampaignEastWest::stage_22_disp_end_game_msg -------//

