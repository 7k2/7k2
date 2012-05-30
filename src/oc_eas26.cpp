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

//Filename   : OC_EAS26.CPP
//Description: CampaignEastWest - STAGE_TRAIN_LEADERS

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
#include <of_inn.h>


//---------- Define static vars ------------//

static int goal_train_unit_leadership, goal_year_limit;


//---- Begin of function CampaignEastWest::stage_26_create_game ----//
//
// Vars defined in event functions before calling this function:
//
// None
//
int CampaignEastWest::stage_26_create_game()
{
	init_random_plot(0);		// no plot

	//--- the player's military is relatively weak in this scenario ---//

	set_std_situation();

	cur_situation.military_level = 45 - campaign_difficulty * 5;
	cur_situation.cash_level = 25;

	//------- create objects and vars --------//

	if( !stage_26_create_nation() )
		return 0;

	if( !stage_26_create_town() )
		return 0;

	if( !stage_26_create_firm() )
		return 0;

	//------ init stage 1 vars -------//

	stage_26_init_vars();

	//---- generate plants and other objects on the map -----//

	world.gen_rocks();		// generate mountains

	world.generate_map2();

	//------- create additional raw sites ----------//

	create_raw_site();

	// ----- create royal units ------//

	create_royal_units(CAMPAIGN_PLAYER_NATION_RECNO);

	//------ plot create game --------//

	(this->*plot_create_game_FP)();

	return 1;
}
//---- End of function CampaignEastWest::stage_26_create_game -----//


//---- Begin of function CampaignEastWest::stage_26_init_vars ----//

void CampaignEastWest::stage_26_init_vars()
{
	//------ set game event vars -------//

	rebel_count = MAX(1, campaign_difficulty-1);
	next_rebel_date = info.game_date + 30 + m.random(30);

	general_defect_count = 5;

	//------- set player nation vars -------//

	init_player_resource(25);

	//-------- add victory conditions -------//

	game.reset_win_condition();
	game.add_win_condition( E_TRAIN_10_GENERAL, 1, 0, goal_train_unit_leadership, goal_year_limit );
}
//---- End of function CampaignEastWest::stage_26_init_vars -----//


//---- Begin of function CampaignEastWest::stage_26_prelude ----//
//
// Pick a state to attack.
//
void CampaignEastWest::stage_26_prelude()
{
	//-------- randomize goal settings --------//

	goal_train_unit_leadership = 90;

	goal_year_limit = 18 - campaign_difficulty * 2;

	//------------ display letter -------------//
	
	play_speech("PRE-26.WAV");
	disp_narrative( res_stage.read("26PRELUD"), goal_train_unit_leadership, goal_year_limit );
	stop_speech();

	target_state_recno = random_pick_state(PLAYER_NATION_RECNO);

	flash_state( target_state_recno );

	//------- set config settings -------//

	config.explore_whole_map = 1;
}
//---- End of function CampaignEastWest::stage_26_prelude -----//


//---- Begin of function CampaignEastWest::stage_26_create_nation ----//

int CampaignEastWest::stage_26_create_nation()
{
	//-------- create the player nation ------//

	state_array[target_state_recno]->add_game_nation();

	return 1;
}
//---- End of function CampaignEastWest::stage_26_create_nation ----//


//---- Begin of function CampaignEastWest::stage_26_create_town ----//

int CampaignEastWest::stage_26_create_town()
{
	//----- create independent towns ------//

	Battle::create_independent_town(3+m.random(2));

	//---------- create towns ---------//

	int townAddCount   = 5 + m.random(3);		// 5 to 7
	int hasFortPercent = 50;

	return create_town(PLAYER_NATION_RECNO, townAddCount, hasFortPercent);
}
//---- End of function CampaignEastWest::stage_26_create_town ----//


//---- Begin of function CampaignEastWest::stage_26_create_firm ----//

int CampaignEastWest::stage_26_create_firm()
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

	//--- while the races of towns are random, the two main towns' race has to be the king's race ---//

	town_array[townRecno1]->set_race(nationPtr->race_id);

	if( townRecno2 )
		town_array[townRecno2]->set_race(nationPtr->race_id);

	if( !create_economic_firm(townRecno1) )
		return 0;

	if( townRecno2 && !create_military_firm(townRecno2) )
		return 0;

	return 1;
}
//---- End of function CampaignEastWest::stage_26_create_firm ----//


//---- Begin of function CampaignEastWest::stage_26_intro_text ----//

char* CampaignEastWest::stage_26_intro_text()
{
	return substitute_text( res_stage.read("26INTRO"), goal_train_unit_leadership, goal_year_limit );
}
//---- End of function CampaignEastWest::stage_26_intro_text ----//


//---- Begin of function CampaignEastWest::stage_26_goal_text ----//

char* CampaignEastWest::stage_26_goal_text()
{
	return substitute_text( res_stage.read("26GOAL"), goal_train_unit_leadership, goal_year_limit );
}
//---- End of function CampaignEastWest::stage_26_goal_text ----//



//=================================================================//


//------- Begin of function CampaignEastWest::stage_26_next_day -------//
//
void CampaignEastWest::stage_26_next_day()
{
	(this->*plot_next_day_FP)();

	if( should_rebel(30) )
		town_defect(PLAYER_NATION_RECNO);

	if( m.random(60+nation_array.nation_count*100)==0 && nation_array.nation_count < campaign_difficulty+1 &&
		 general_defect_count > 0 ) 		// stop forming new nations when a specific number of new nations have already been formed.
	{
		general_defect_count--;
		rebel_form_nation();       // when this function is called, a new nation will formed if there is any independent towns.
	}

	//------ remove units from inn -------//

	if( info.game_date % 15 == 0 )
	{
		for( int i=firm_array.size() ; i>0 ; i-- )
		{
			if( firm_array.is_deleted(i) )
				continue;

			FirmInn* firmPtr = firm_array[i]->cast_to_FirmInn();

			if( firmPtr && firmPtr->inn_unit_count > 0 )
			{
				firmPtr->del_inn_unit( m.random( firmPtr->inn_unit_count ) + 1 );
				break;
			}
		}
	}
}
//------- End of function CampaignEastWest::stage_26_next_day --------//


//============================================================//


//--- Begin of function CampaignEastWest::stage_26_process_game_result ---//
//
void CampaignEastWest::stage_26_process_game_result()
{
	//--- if the player won the game -----//

	if( game.result_win_condition_id >= 0 )
	{
		err_when( !return_event_id );

		set_event( return_event_id );
	}

	//---- if the player lost the game ----//

	else
	{
		// do nothing as the player must retry the game.

		info.game_year = game_year;			// revert info.game_year back to campaign's game year so that the game's time will not advance

		m.set_random_seed( saved_random_seed );		// restore the random seed so that the game will replay in the same way
	}
}
//---- End of function CampaignEastWest::stage_26_process_game_result -----//


//------ Begin of function CampaignEastWest::stage_26_disp_end_game_msg ------//
//
void CampaignEastWest::stage_26_disp_end_game_msg()
{
	if( game.result_win_condition_id >= 0 )
	{
		disp_in_game_msg( res_stage.read("26WIN") );
	}
	else
	{
		disp_in_game_msg( res_stage.read("26LOSE") );
	}
}
//------- End of function CampaignEastWest::stage_26_disp_end_game_msg -------//


//------ Begin of function CampaignEastWest::stage_26_write_file ------//
//
void CampaignEastWest::stage_26_write_file(File* filePtr)
{
	filePtr->file_put_long(goal_year_limit);
	filePtr->file_put_long(goal_train_unit_leadership);
}
//------- End of function CampaignEastWest::stage_26_write_file -------//


//------ Begin of function CampaignEastWest::stage_26_read_file ------//
//
void CampaignEastWest::stage_26_read_file(File* filePtr)
{
	goal_year_limit = filePtr->file_get_long();
	goal_train_unit_leadership = filePtr->file_get_long();
}
//------- End of function CampaignEastWest::stage_26_read_file -------//

