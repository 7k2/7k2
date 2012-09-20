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

//Filename   : OC_EAST4.CPP
//Description: CampaignEastWest - STAGE_BATTLE_EAST

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

#define STAGE_4_EASTERN_NATION_RECNO	2


//---- Begin of function CampaignEastWest::stage_4_create_game ----//
//
// Vars defined in event functions before calling this function:
//
// -eastern_nation_recno
// -target_state_recno
//
int CampaignEastWest::stage_4_create_game()
{
	init_random_plot('C');

   //----------------------------------//

	if( !stage_4_create_nation() )
		return 0;

	if( !stage_4_create_town() )
		return 0;

	if( !stage_4_create_firm() )
		return 0;

	//------ create the player troop -------//

	create_troop(PLAYER_NATION_RECNO, 65-campaign_difficulty*10, 65-campaign_difficulty*5);

	//------ init stage 1 vars -------//

	stage_4_init_vars();

	//---- generate plants and other objects on the map -----//

	world.gen_rocks();

	world.generate_map2();

	//------- create additional raw sites ----------//

	create_raw_site();

	//------- init relationship -------//

	stage_4_init_relation();

	// ----- create royal units ------//

	create_royal_units(CAMPAIGN_PLAYER_NATION_RECNO);

	//------ plot create game --------//

	(this->*plot_create_game_FP)();

	//---- randomly unveil the locations of some of the Eastern Empire's towns ----//

	for( int i=town_array.size() ; i>0 ; i-- )
	{
		if( town_array.is_deleted(i) )
			continue;

		Town* townPtr = town_array[i];

		if( townPtr->nation_recno == STAGE_4_EASTERN_NATION_RECNO &&
			 misc.random(2)==0 )
		{
			world.unveil( townPtr->loc_x1, townPtr->loc_y1, townPtr->loc_x2, townPtr->loc_y2 );
			world.visit( townPtr->loc_x1, townPtr->loc_y1, townPtr->loc_x2, townPtr->loc_y2, EXPLORE_RANGE-1 );
		}
	}

	return 1;
}
//---- End of function CampaignEastWest::stage_4_create_game -----//


//---- Begin of function CampaignEastWest::stage_4_init_vars ----//

void CampaignEastWest::stage_4_init_vars()
{
	rebel_count=0;
	next_rebel_date = info.game_date + 90 + misc.random(60);		// don't rebel until at least 3 months into the game

	//------- set player nation vars -------//

	init_player_resource(40);

	init_nation_resource(STAGE_4_EASTERN_NATION_RECNO, 35);
}
//---- End of function CampaignEastWest::stage_4_init_vars -----//


//---- Begin of function CampaignEastWest::stage_4_prelude ----//
//
// Pick a state to attack.
//
void CampaignEastWest::stage_4_prelude()
{
	play_speech("PRE-04.WAV");
	disp_narrative( res_stage.read("4PRELUDE") );		// 0-the letter is from a human kingdom
	stop_speech();

	attacker_state_recno = random_pick_state( western_nation_recno, target_state_recno );		// pick a state of the Western empire that is next to defected state.

	attack_state( attacker_state_recno, target_state_recno, 0, 1, 2 );

	//------- set config settings -------//

	config.explore_whole_map = 0;
}
//---- End of function CampaignEastWest::stage_4_prelude -----//


//---- Begin of function CampaignEastWest::stage_4_create_nation ----//

int CampaignEastWest::stage_4_create_nation()
{
	state_array[attacker_state_recno]->add_game_nation();
	state_array[target_state_recno]->add_game_nation();

	return 1;
}
//---- End of function CampaignEastWest::stage_4_create_nation ----//


//---- Begin of function CampaignEastWest::stage_4_create_town ----//

int CampaignEastWest::stage_4_create_town()
{
	int townAddCount   = 5 + misc.random(3);
	int hasFortPercent = 50;

	return create_town(STAGE_4_EASTERN_NATION_RECNO, townAddCount, hasFortPercent);
}
//---- End of function CampaignEastWest::stage_4_create_town ----//


//---- Begin of function CampaignEastWest::stage_4_create_firm ----//

int CampaignEastWest::stage_4_create_firm()
{
	//----- create firms around established towns -----//

	Nation* nationPtr = nation_array[STAGE_4_EASTERN_NATION_RECNO];

	int townRecno1 = king_oversee_town_recno(STAGE_4_EASTERN_NATION_RECNO);
	int townRecno2 = random_pick_town_with_camp(STAGE_4_EASTERN_NATION_RECNO, 2);		// only pick towns with <= 2 links

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

	//--- set other towns to your race, as they were yours originally ---//

	int playerRaceId = nation_array[PLAYER_NATION_RECNO]->race_id;

	for( int i=town_array.size() ; i>0 ; i-- )
	{
		if( town_array.is_deleted(i) )
			continue;

		town_array[i]->set_race(playerRaceId);
	}

	//----------------------------------------//

	if( !create_economic_firm(townRecno1) )
		return 0;

	if( townRecno2 && !create_military_firm(townRecno2) )
		return 0;

	return 1;
}
//---- End of function CampaignEastWest::stage_4_create_firm ----//


//------- Begin of function CampaignEastWest::stage_4_init_relation -------//
//
void CampaignEastWest::stage_4_init_relation()
{
	set_nation_hostile(PLAYER_NATION_RECNO, STAGE_4_EASTERN_NATION_RECNO);
}
//------- End of function CampaignEastWest::stage_4_init_relation --------//


//=================================================================//


//------- Begin of function CampaignEastWest::stage_4_next_day -------//
//
void CampaignEastWest::stage_4_next_day()
{
	//------ plot next day ----------//

	(this->*plot_next_day_FP)();

	//--- towns of the eastern empire will defect to the player's nation as player's nation grows stronger ---//

	if( info.game_date >= next_rebel_date &&
		 !nation_array.is_deleted(STAGE_4_EASTERN_NATION_RECNO) )
	{
		int const maxRebelCount = 5;		// maximum 5 rebellions in the Eastern Empire

		int playerRating = nation_array[PLAYER_NATION_RECNO]->overall_rating;
		int easternRating = nation_array[STAGE_4_EASTERN_NATION_RECNO]->overall_rating;

		if( maxRebelCount * playerRating / easternRating > rebel_count ||
			 campaign_difficulty==1 )		// If difficulty is 1, then don't check the rating
		{
			int townRecno = town_defect( STAGE_4_EASTERN_NATION_RECNO, PLAYER_NATION_RECNO );

			if( townRecno )
			{
				town_array[townRecno]->loyalty = (float) (80 - campaign_difficulty*5);
				town_array[townRecno]->bonus_loyalty = 30 - campaign_difficulty*5;
			}

			rebel_count++;
		}
	}
}
//------- End of function CampaignEastWest::stage_4_next_day --------//


//============================================================//


//--- Begin of function CampaignEastWest::stage_4_process_game_result ---//
//
void CampaignEastWest::stage_4_process_game_result()
{
	//--- if the player won the game -----//

	if( game.result_win_condition_id >= 0 )
	{
		attack_state( attacker_state_recno, target_state_recno, 1, 2, 3 );

		set_event( EVENT_RANDOM_POINT_1 );
	}

	//---- if the player lost the game ----//

	else
	{
		set_event( EVENT_RANDOM_POINT_1 );		// still go to random point 1 even the player has lost the game.
	}
}
//---- End of function CampaignEastWest::stage_4_process_game_result -----//


//------ Begin of function CampaignEastWest::stage_4_disp_end_game_msg ------//
//
void CampaignEastWest::stage_4_disp_end_game_msg()
{
	if( game.result_win_condition_id >= 0 )
	{
		disp_in_game_msg( res_stage.read("4WIN") );
	}
	else
	{
		disp_in_game_msg( res_stage.read("4LOSE") );
	}
}
//------- End of function CampaignEastWest::stage_4_disp_end_game_msg -------//

