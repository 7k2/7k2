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

//Filename   : OC_EAS23.CPP
//Description: CampaignEastWest - STAGE_LAST_BATTLE

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
#include <of_camp.h>

//------ Define constant ------//

#define STAGE_23_EASTERN_NATION_RECNO	2


//---- Begin of function CampaignEastWest::stage_23_create_game ----//
//
// Vars defined in event functions before calling this function:
//
// -new_kingdom_nation_recno
// -attack_state_recno
// -target_state_recno
//
int CampaignEastWest::stage_23_create_game()
{
	init_random_plot('C');

	if( !stage_23_create_nation() )
		return 0;

	if( !stage_23_create_town() )
		return 0;

	if( !stage_23_create_firm() )
		return 0;

	//----- create offensive structures ---------//

	create_offensive_structure( STAGE_23_EASTERN_NATION_RECNO, 1+campaign_difficulty/2 + m.random(2) );		// build 2 structures if the difficulty > 3

	//------ create the player troop -------//

	create_troop(PLAYER_NATION_RECNO, 86-campaign_difficulty*6);

	//------ init stage 1 vars -------//

	stage_23_init_vars();

	//---- generate plants and other objects on the map -----//

	world.gen_rocks();

	world.generate_map2();

	//------- create additional raw sites ----------//

	create_raw_site();

	//------- init relationship -------//

	stage_23_init_relation();

	// ####### begin Gilbert 9/4 ########//
	// ----- create royal units ------//

	create_royal_units(CAMPAIGN_PLAYER_NATION_RECNO);
	// ####### end Gilbert 9/4 ########//

	(this->*plot_create_game_FP)();

	return 1;
}
//---- End of function CampaignEastWest::stage_23_create_game -----//


//---- Begin of function CampaignEastWest::stage_23_init_vars ----//

void CampaignEastWest::stage_23_init_vars()
{
	//------- set player nation vars -------//

	init_player_resource(50);

	init_nation_resource(STAGE_23_EASTERN_NATION_RECNO, 100);
}
//---- End of function CampaignEastWest::stage_23_init_vars -----//


//---- Begin of function CampaignEastWest::stage_23_prelude ----//
//
// Pick a state to attack.
//
void CampaignEastWest::stage_23_prelude()
{
	play_speech("PRE-23.WAV");
	disp_narrative( res_stage.read("23PRELUD") );
	stop_speech();

	attack_state( attacker_state_recno, target_state_recno, 0, 1, 2 );

	//------- set config settings -------//

	config.explore_whole_map = 0;
}
//---- End of function CampaignEastWest::stage_23_prelude -----//


//---- Begin of function CampaignEastWest::stage_23_create_nation ----//

int CampaignEastWest::stage_23_create_nation()
{
	state_array[attacker_state_recno]->add_game_nation();
	state_array[target_state_recno]->add_game_nation();

	return 1;
}
//---- End of function CampaignEastWest::stage_23_create_nation ----//


//---- Begin of function CampaignEastWest::stage_23_create_town ----//

int CampaignEastWest::stage_23_create_town()
{
	int townAddCount   = 8 + m.random(2);
	int hasFortPercent = 100;
	int combatLevel    = 40 + campaign_difficulty*10;

	return create_town( STAGE_23_EASTERN_NATION_RECNO, townAddCount, hasFortPercent, combatLevel);
}
//---- End of function CampaignEastWest::stage_23_create_town ----//


//---- Begin of function CampaignEastWest::stage_23_create_firm ----//

int CampaignEastWest::stage_23_create_firm()
{
	//----- create firms around established towns -----//

	Nation* nationPtr = nation_array[STAGE_23_EASTERN_NATION_RECNO];

	int townRecno1 = king_oversee_town_recno(STAGE_23_EASTERN_NATION_RECNO);
	int townRecno2 = random_pick_town_with_camp(STAGE_23_EASTERN_NATION_RECNO, 2);		// only pick towns with <= 2 links

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

	create_economic_firm(townRecno1);
	create_offensive_structure(townRecno1);

	if( townRecno2 )
	{
		create_military_firm(townRecno2);
		create_offensive_structure(townRecno2);
	}

	//----- four possible situations -------//

	int eastSituation = m.random(2)+1;

	//-------------------------------------------------//
	// The target state has an emphasis on weapon
	// development and production. There are a number
	// of war factories and a large number of weapons.
	//-------------------------------------------------//

	if( eastSituation==1 )
	{
		//---- build a war factory next to the town with mostly economic facilities.

		create_work_firm( town_array[townRecno1], FIRM_WAR_FACTORY );

		//-- fill most of the forts with weapons -----//

		for( int i=0 ; i<nationPtr->ai_camp_count ; i++ )
		{
			FirmCamp* firmCamp = firm_array[ nationPtr->ai_camp_array[i] ]->cast_to_FirmCamp();

			int addCount = MAX_SOLDIER - firmCamp->soldier_count - m.random(3);		// don't always full up, sometimes leave a free slots empty

			for( int j=addCount ; j>0 ; j-- )
				add_weapon_to_camp( firmCamp->firm_recno );
		}

		//---- give it more cash upfront ------//

		nationPtr->cash += 2300 + 1000 * campaign_difficulty;
	}

	//-------------------------------------------------//
	// The target state has an emphasis on economic
	// development, it is vastly rich. And can use money
	// to boost up its military rapidly.
	//-------------------------------------------------//

	if( eastSituation==2 )
	{
		//--- create one more series of economic buildings ---//

		int townRecno2 = random_pick_town_with_camp(STAGE_23_EASTERN_NATION_RECNO, 2);		// only pick towns with <= 2 links

		if( townRecno2 )
			create_economic_firm(townRecno2);

		//---- give it more cash upfront ------//

		nationPtr->cash += 3000 + 2300 * campaign_difficulty;
	}

	//------- add more groups of economy and military structures ---//

	int townRecno;

	if( campaign_difficulty >= 3 )
	{
		townRecno = random_pick_town_with_camp(STAGE_23_EASTERN_NATION_RECNO, 2);		// only pick towns with <= 2 links

		if( townRecno )
		{
			town_array[townRecno]->set_race(nationPtr->race_id);
			create_military_firm(townRecno);
			create_offensive_structure(townRecno);
		}
	}

	if( campaign_difficulty >= 4 )
	{
		townRecno = random_pick_town_with_camp(STAGE_23_EASTERN_NATION_RECNO, 2);		// only pick towns with <= 2 links

		if( townRecno )
		{
			town_array[townRecno]->set_race(nationPtr->race_id);
			create_economic_firm(townRecno);
			create_offensive_structure(townRecno);
		}
	}

	if( campaign_difficulty >= 5 )
	{
		townRecno = random_pick_town_with_camp(STAGE_23_EASTERN_NATION_RECNO, 2);		// only pick towns with <= 2 links

		if( townRecno )
		{
			town_array[townRecno]->set_race(nationPtr->race_id);
			create_military_firm(townRecno);
			create_offensive_structure(townRecno);
      }
	}

	return 1;
}
//---- End of function CampaignEastWest::stage_23_create_firm ----//


//------- Begin of function CampaignEastWest::stage_23_init_relation -------//
//
void CampaignEastWest::stage_23_init_relation()
{
	set_nation_hostile(PLAYER_NATION_RECNO, STAGE_23_EASTERN_NATION_RECNO);
}
//------- End of function CampaignEastWest::stage_23_init_relation --------//


//=================================================================//


//------- Begin of function CampaignEastWest::stage_23_next_day -------//
//
void CampaignEastWest::stage_23_next_day()
{
	//------ plot next day ----------//

	(this->*plot_next_day_FP)();
}
//------- End of function CampaignEastWest::stage_23_next_day --------//


//============================================================//


//--- Begin of function CampaignEastWest::stage_23_process_game_result ---//
//
void CampaignEastWest::stage_23_process_game_result()
{
	//--- if the player won the game -----//

	if( game.result_win_condition_id >= 0 )
	{
		attack_state( attacker_state_recno, target_state_recno, 1, 2, 3 );

		set_event( EVENT_ULTIMATE_VICTORY );
	}

	//---- if the player lost the game ----//

	else
	{
		//-- the Eastern empire invades the player and takes a state of it --//

		if( random_pick_attack_state( eastern_nation_recno, western_nation_recno ) )
		{
			disp_narrative( res_stage.read("23POST") );
			attack_state( attacker_state_recno, target_state_recno, 1, 1, 3 );
		}

		set_event( EVENT_RANDOM_POINT_4 );
	}
}
//---- End of function CampaignEastWest::stage_23_process_game_result -----//


//------ Begin of function CampaignEastWest::stage_23_disp_end_game_msg ------//
//
void CampaignEastWest::stage_23_disp_end_game_msg()
{
	if( game.result_win_condition_id >= 0 )
	{
		disp_in_game_msg( res_stage.read("23WIN") );
	}
	else
	{
		disp_in_game_msg( res_stage.read("23LOSE") );
	}
}
//------- End of function CampaignEastWest::stage_23_disp_end_game_msg -------//

